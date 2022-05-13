#include "output.cginc"

struct VS_INPUT {
  float3 m_Position :     v_Position;
  float3 m_Normal :       v_Normal;
  float3 m_Tangent :      v_Tangent;
  float  m_Sign :         v_Sign;
  float2 m_TexCoord :     v_TexCoord;
};

cbuffer ViewBuffer :      register(b0) {
  float4x4 view;
  float3 eye;
};

cbuffer ModelBuffer :     register(b1) {
  float4x4 model;
};

VS_OUTPUT main(VS_INPUT input) {
  VS_OUTPUT output;
  //output.m_PositionWS =   float4(input.m_Position, 1.0);
  output.m_PositionWS =   mul(model, float4(input.m_Position, 1.0)).xyz;
  output.m_PositionCS =   mul(mul(view, model), float4(input.m_Position, 1.0));
  output.m_Position =     output.m_PositionCS;
  output.m_Normal =       mul((float3x3)model, input.m_Normal);
  output.m_Tangent =      mul((float3x3)model, input.m_Tangent);
  output.m_Sign =         input.m_Sign;
  output.m_TexCoord =     input.m_TexCoord;
  output.m_View =         normalize(eye - output.m_PositionWS);
  return                  output;
}