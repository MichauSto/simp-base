// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 ACESFilm(float3 x)
{
  float a = 2.51f;
  float b = 0.03f;
  float c = 2.43f;
  float d = 0.59f;
  float e = 0.14f;
  return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

float3 exposure(float3 color, float val) {
  return color * pow(2.0, val);
}

float3 tonemap(float3 color) {
  return ACESFilm(color);
}

float3 LinearToSRGB(float3 color) {
  return pow(max(0, color), 1 / 2.2);
}

float3 SRGBToLinear(float3 color) {
  return pow(max(0, color), 2.2);
}