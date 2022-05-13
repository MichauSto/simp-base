#include "materialcomponents.hpp"

#include "scriptcomponents.hpp"
#include "simp.hpp"
#include "visual/shaders.hpp"

namespace simp {

  void Scene::UpdateMaterials(ID3D11DeviceContext* context)
  {
    {
      auto view = Registry.view<MaterialChangeComponent, MaterialComponent>();
      for (auto e : view) {
        auto [matlChange, matl] = view.get(e);
        auto controller = Registry.get<ScriptComponent>(matlChange.Controller);
        int index = (int)controller.VarList[matlChange.VarIndex];
        if (index < 0 || index >= matlChange.Materials.size())
          index = -1;
        if (index != matlChange.CachedIndex) {
          matlChange.CachedIndex = index;
          matl.m_Material = index >= 0 ? matlChange.Materials[index] : matlChange.Default;
        }
      }
    }

    {
      auto view = Registry.view<MaterialComponent, MaterialDirtyComponent>();
      for (auto e : view) {
        auto [matl, dirty] = view.get(e);
        D3D11_MAPPED_SUBRESOURCE msrMaterial{};
        context->Map(
          matl.m_Material.MaterialBuffer.Get(),
          0,
          D3D11_MAP_WRITE_DISCARD,
          0,
          &msrMaterial);

        *static_cast<Material::MaterialData*>(msrMaterial.pData) = matl.m_Material.Data;

        context->Unmap(
          matl.m_Material.MaterialBuffer.Get(),
          0);
      }
    }
  }

  void Material::CreateResources()
  {
    // Update feature flags
    for (int i = 0; i < MapCount; ++i) {
      Data.FeatureFlags[i] = (bool)Textures[i];
    }
    Data.FeatureFlags[AlphaTest] = AlphaMode == 1;
    Data.FeatureFlags[AlphaBlend] = AlphaMode == 2;

    // Create constant buffer
    D3D11_BUFFER_DESC mbDesc{};
    mbDesc.ByteWidth = sizeof(Data);
    mbDesc.Usage = D3D11_USAGE_DYNAMIC;
    mbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    mbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    mbDesc.MiscFlags = 0;
    mbDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA mbData{};
    mbData.pSysMem = &Data;

    Simp::GetGraphics().GetDevice()->CreateBuffer(&mbDesc, &mbData, &MaterialBuffer);

    // Create Depth-Stencil state
    D3D11_DEPTH_STENCIL_DESC dsDesc{};
    dsDesc.DepthEnable = true;// !ZbufCheckDisable;
    dsDesc.DepthWriteMask = ZbufWriteDisable ? D3D11_DEPTH_WRITE_MASK_ZERO : D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    Simp::GetGraphics().GetDevice()->CreateDepthStencilState(&dsDesc, &DepthState);

    // Assign blend state
    BlendState = AlphaMode == 2 ? Shaders::GetBlendStateAlphaBlend() : nullptr;

    // Create sampler state
    D3D11_SAMPLER_DESC msDesc{};
    
    msDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    msDesc.AddressU = WrapMode;
    msDesc.AddressV = WrapMode;
    msDesc.AddressW = WrapMode;
    msDesc.MipLODBias = 0.f;
    msDesc.MaxAnisotropy = 16;
    msDesc.MinLOD = 0;
    msDesc.MaxLOD = D3D11_FLOAT32_MAX;
    *reinterpret_cast<glm::vec4*>(msDesc.BorderColor) = BorderColor;
    
    Simp::GetGraphics().GetDevice()->CreateSamplerState(&msDesc, &Samplers[0]);
    Samplers[1] = Shaders::GetEnvMapSampler();
  }

  void Material::Use(ID3D11DeviceContext* context) const
  {
    context->PSSetConstantBuffers(
      1, 
      1, 
      MaterialBuffer.GetAddressOf());
    context->OMSetDepthStencilState(
      DepthState.Get(), 
      0);
    context->OMSetBlendState(
      BlendState.Get(), 
      nullptr, 
      0x0f);
    context->PSSetSamplers(
      0, 
      2, 
      (ID3D11SamplerState**)Samplers.data());
    context->PSSetShaderResources(
      0,
      Textures.size(), 
      (ID3D11ShaderResourceView**)Textures.data());
  }


}