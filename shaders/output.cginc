
struct VS_OUTPUT {
  float4 m_Position :     SV_Position;
  float4 m_PositionCS :   PositionCS;
  float3 m_PositionWS :   Position;
  float3 m_Normal :       Normal;
  float3 m_Tangent :      Tangent;
  float  m_Sign :         Sign;
  float2 m_TexCoord :     TexCoord;
  float3 m_View :         View;
};
