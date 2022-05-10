#include "output.cginc"

SamplerState texSampler :     register(s0);
SamplerState envSampler :     register(s1);

Texture2D TexDiffuse :        register(t0);
Texture2D TexEnvMap :         register(t1);
Texture2D TexBumpmap :        register(t2);
Texture2D TexTransmap :       register(t3);
Texture2D TexEnvmapMask :     register(t4);
Texture2D TexLightmap :       register(t5);
Texture2D TexNightmap :       register(t6);
Texture2D TexSpecGloss :      register(t7);
Texture2D TexNormal :         register(t8);

#define FEAT_DIFFUSE         (1 << 0)
#define FEAT_ENVMAP          (1 << 1)
#define FEAT_BUMPMAP         (1 << 2)
#define FEAT_TRANSMAP        (1 << 3)
#define FEAT_ENVMASK         (1 << 4)
#define FEAT_LIGHTMAP        (1 << 5)
#define FEAT_NIGHTMAP        (1 << 6)
#define FEAT_SPECGLOSS       (1 << 7)
#define FEAT_NORMALMAP       (1 << 8)
#define FEAT_ALPHATEST       (1 << 10)
#define FEAT_ALPHABLEND      (1 << 11)

cbuffer MaterialBuffer :      register(b1) {
  float3 Diffuse;
  float Alpha;
  float3 Specular;
  float Shininess;
  float3 Emissive;
  float3 Ambient;
  uint FeatureFlags;
  float EnvMapIntensity;
  float BumpMapIntensity;
  float LightMapIntensity;
};

float3 SampleNormalFromBumpmap(float f, float2 tc, Texture2D o, SamplerState s) {
  float3 size;
  o.GetDimensions(0, size.x, size.y, size.z);
  float2 deltaX = float2(1.0, 0.0) / (float2)size;
  float2 deltaY = float2(0.0, 1.0) / (float2)size;
  float dx = f * (o.Sample(s, tc + deltaX).r - o.Sample(s, tc - deltaX).r);
  float dy = f * (o.Sample(s, tc + deltaY).r - o.Sample(s, tc - deltaY).r);
  return normalize(float3(dx, dy, 1.0));
}

float3x3 getTBN(in VS_OUTPUT input) {
  float3 b = input.m_Sign * cross(input.m_Normal, input.m_Tangent);
  return float3x3(input.m_Tangent, b, input.m_Normal);
}

float4 main(VS_OUTPUT input) : SV_Target0
{
  float4 DiffuseSample =      TexDiffuse.Sample(texSampler, input.m_TexCoord);
  float3 Color =              DiffuseSample.rgb;

  bool HasDiffuse =           FeatureFlags & FEAT_DIFFUSE;
  bool HasEnvMap =            FeatureFlags & FEAT_ENVMAP;
  bool HasBumpMap =           FeatureFlags & FEAT_BUMPMAP;
  bool HasTransMap =          FeatureFlags & FEAT_TRANSMAP;
  bool HasEnvMask =           FeatureFlags & FEAT_ENVMASK;
  bool HasLightMap =          FeatureFlags & FEAT_LIGHTMAP;
  bool HasNightMap =          FeatureFlags & FEAT_NIGHTMAP;
  bool HasSpecGlossMap =      FeatureFlags & FEAT_SPECGLOSS;
  bool HasNormalMap =         FeatureFlags & FEAT_NORMALMAP;
  bool HasAlphaTest =         FeatureFlags & FEAT_ALPHATEST;
  bool HasAlphaBlend =        FeatureFlags & FEAT_ALPHABLEND;

  float Alpha;

  if (HasTransMap) {
    Alpha = TexTransmap.Sample(texSampler, input.m_TexCoord).a;
  }
  else {
    Alpha = DiffuseSample.a;
  }

  if (HasAlphaTest && Alpha < 0.5) {
    discard;
  }

  float3 Normal;

  if (HasNormalMap) {
    float2 NormalSample = TexNormal.Sample(texSampler, input.m_TexCoord).rg;
    NormalSample = NormalSample * 2.0 - 1.0;
    Normal = mul(getTBN(input), float3(NormalSample, sqrt(1.0 - dot(NormalSample, NormalSample))));
  }
  else if (HasBumpMap) {
    Normal = mul(getTBN(input), SampleNormalFromBumpmap(BumpMapIntensity, input.m_TexCoord, TexBumpmap, texSampler));
  }
  else {
    Normal = input.m_Normal;
  }

  float3 Reflect = reflect(normalize(input.m_Eye - input.m_PositionWS), Normal);
  float3 Halfway = normalize(float3(0.0, 0.0, 1.0) + input.m_Eye - input.m_PositionWS);

  Color *= max(0.0, dot(Normal, float3(0.0, 0.0, 1.0))) * Diffuse + Ambient;

  if (HasEnvMap) {
    float EnvBlend =          EnvMapIntensity;
    if (HasEnvMask) {
      EnvBlend =              TexEnvmapMask.Sample(texSampler, input.m_TexCoord).a;
    }
    else if (HasTransMap) {
      EnvBlend =              DiffuseSample.a;
    }
    Color =                   lerp(Color, Ambient * TexEnvMap.Sample(envSampler, Reflect.xz * float2(0.5, -0.5) + 0.5).rgb, EnvBlend);
  }

  Color += pow(max(0.0, dot(Halfway, Normal)), Shininess) * Specular;

  return float4(Color, Alpha);

}