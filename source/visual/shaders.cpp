#include "shaders.hpp"
#include "simp.hpp"
#include "mesh.hpp"

namespace simp {

  namespace {
#include "shaders/static.vert.hpp"
#include "shaders/legacy.frag.hpp"
  }

  namespace {
    const D3D11_INPUT_ELEMENT_DESC shaderIl_static[] = {
      { "v_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, Vertex::Position),  D3D11_INPUT_PER_VERTEX_DATA, 0},
      { "v_Normal",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, Vertex::Normal),    D3D11_INPUT_PER_VERTEX_DATA, 0},
      { "v_Tangent",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, Vertex::Tangent),   D3D11_INPUT_PER_VERTEX_DATA, 0},
      { "v_Sign",     0, DXGI_FORMAT_R32_FLOAT,       0, offsetof(Vertex, Vertex::Sign),      D3D11_INPUT_PER_VERTEX_DATA, 0},
      { "v_TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(Vertex, Vertex::TexCoord),  D3D11_INPUT_PER_VERTEX_DATA, 0}};
  }

  ID3D11VertexShader* Shaders::GetVertexStatic()
  {
    assert(Instance);
    return Instance->VsStatic.Get();
  }

  ID3D11InputLayout* Shaders::GetInputLayoutStatic()
  {
    assert(Instance);
    return Instance->ILStatic.Get();
  }

  ID3D11PixelShader* Shaders::GetPixelLegacy()
  {
    assert(Instance);
    return Instance->PsLegacy.Get();
  }

  uint32_t* Shaders::GetStridesStatic()
  {
    static uint32_t strides[]{ sizeof(Vertex) };
    return strides;
  }

  uint32_t* Shaders::GetOffsetsStatic()
  {
    static uint32_t offsets[]{ 0 };
    return offsets;
  }

  ID3D11BlendState* Shaders::GetBlendStateAlphaBlend()
  {
    assert(Instance);
    return Instance->BsAlphaBlend.Get();
  }

  ID3D11SamplerState* Shaders::GetEnvMapSampler()
  {
    assert(Instance);
    return Instance->EnvMapSampler.Get();
  }

  Shaders::Shaders()
  {
    assert(!Instance);
    Instance = this;

    ComPtr<ID3D11Device> device = Simp::GetGraphics().GetDevice();

    device->CreateInputLayout(
      shaderIl_static, 
      ARRAYSIZE(shaderIl_static), 
      shader_static_vert, 
      ARRAYSIZE(shader_static_vert), 
      &ILStatic);
    device->CreateVertexShader(
      shader_static_vert, 
      ARRAYSIZE(shader_static_vert), 
      nullptr, 
      &VsStatic);
    device->CreatePixelShader(
      shader_legacy_frag, 
      ARRAYSIZE(shader_legacy_frag), 
      nullptr, 
      &PsLegacy);

    D3D11_BLEND_DESC bsDesc{};

    bsDesc.IndependentBlendEnable = false;
    bsDesc.RenderTarget[0].BlendEnable = true;
    bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bsDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bsDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bsDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    bsDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bsDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

    device->CreateBlendState(&bsDesc, &BsAlphaBlend);

    D3D11_SAMPLER_DESC envSamplerDesc{};
    envSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    envSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    envSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    envSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    envSamplerDesc.MipLODBias = 0.f;
    envSamplerDesc.MaxAnisotropy = 16;
    envSamplerDesc.MinLOD = 0;
    envSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    device->CreateSamplerState(&envSamplerDesc, &EnvMapSampler);
  }

  Shaders::~Shaders()
  {
    assert(Instance == this);
    Instance = nullptr;
  }

  const Shaders* Shaders::Instance = nullptr;
}