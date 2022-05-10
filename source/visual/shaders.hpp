#pragma once

#include "utils/Windows.hpp"

namespace simp {

  struct Shaders {
  public:
    static ID3D11VertexShader* GetVertexStatic();
    static ID3D11InputLayout* GetInputLayoutStatic();
    static ID3D11PixelShader* GetPixelLegacy();
    static uint32_t* GetStridesStatic();
    static uint32_t* GetOffsetsStatic();
    static ID3D11BlendState* GetBlendStateAlphaBlend();
    static ID3D11SamplerState* GetEnvMapSampler();
    Shaders();
    ~Shaders();
  private:
    static const Shaders* Instance;
    ComPtr<ID3D11VertexShader> VsStatic;
    ComPtr<ID3D11InputLayout> ILStatic;
    ComPtr<ID3D11PixelShader> PsLegacy;
    ComPtr<ID3D11BlendState> BsAlphaBlend;
    ComPtr<ID3D11SamplerState> EnvMapSampler;
  };

}