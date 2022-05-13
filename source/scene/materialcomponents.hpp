#pragma once

#include "scene.hpp"
#include "visual/mesh.hpp"
#include <bitset>

namespace simp {

  struct Material {

    enum FeatureIndex : uint32_t {
      Diffuse,
      Envmap,
      Bumpmap,
      Transmap,
      EnvmapMask,
      Lightmap,
      Nightmap,
      SpecGloss,
      Normalmap,

      MapCount,

      AlphaTest,
      AlphaBlend
    };

    // Material constant buffer
    struct MaterialData {
      alignas(16) glm::vec3       Diffuse;
                  float           Alpha;
      alignas(16) glm::vec3       Specular;
                  float           Shininess;
      alignas(16) glm::vec3       Emissive;
      alignas(16) glm::vec3       Ambient;
                  std::bitset<32> FeatureFlags;
                  float           EnvMapIntensity;
                  float           BumpMapIntensity;
                  float           LightMapIntensity;
    } Data;

    // Texture sampler parameters
    D3D11_TEXTURE_ADDRESS_MODE WrapMode = D3D11_TEXTURE_ADDRESS_WRAP;
    glm::vec4 BorderColor;

    // Blending and depth state options
    int AlphaMode;
    bool ZbufCheckDisable;
    bool ZbufWriteDisable;

    // D3D11 Resources
    ComPtr<ID3D11Buffer> MaterialBuffer;
    ComPtr<ID3D11BlendState> BlendState;
    ComPtr<ID3D11DepthStencilState> DepthState;
    std::array<ComPtr<ID3D11ShaderResourceView>, MapCount> Textures;
    std::array<ComPtr<ID3D11SamplerState>, 2> Samplers;

    void CreateResources();

    void Use(ID3D11DeviceContext* context) const;
  };

  struct MaterialDirtyComponent {
    int Dummy;
  };

  struct MaterialChangeComponent {
    entt::entity Controller;
    int VarIndex;
    Material Default;
    std::vector<Material> Materials;
    int CachedIndex;
  };

  struct MaterialComponent {
    Material m_Material;
  };

}