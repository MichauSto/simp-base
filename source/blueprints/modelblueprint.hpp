#pragma once

#include "visual/mesh.hpp"
#include "visual/texture.hpp"
#include "filesystem/cfgfile.hpp"
#include "utils/dict.hpp"

#include "scene/scene.hpp"
#include "scene/materialcomponents.hpp"

#include <variant>
#include <memory>
#include <vector>
#include <filesystem>

namespace simp {



  struct MaterialBlueprint {
    struct TextTexture {
      int Index;
    };
    struct ScriptTexture {
      int Index;
    };
    using TextureEntry = std::variant<std::shared_ptr<Texture>, TextTexture, ScriptTexture>;
    struct Item {
      Material GetMaterial(
        entt::entity controller,
        std::function<std::shared_ptr<Texture>(int)> getTextTexture,
        std::function<std::shared_ptr<Texture>(int)> getScriptTexture) const;
      std::array<TextureEntry, Material::MapCount> Textures;
      int AlphaMode;
      bool ZbufCheckDisable;
      bool ZbufWriteDisable;
      D3D11_TEXTURE_ADDRESS_MODE WrapMode = D3D11_TEXTURE_ADDRESS_WRAP;
      glm::vec4 BorderColor;
      glm::vec3 Diffuse;
      float Alpha;
      glm::vec3 Specular;
      float Shininess;
      glm::vec3 Emissive;
      glm::vec3 Ambient;
      float EnvMapIntensity;
      float BumpMapIntensity;
      float LightMapIntensity;
    };
    Item Default{};
    std::vector<Item> Items{};
    int varIndex = -1;
    static TextureEntry ParseTexture(
      const std::string_view& name, 
      const std::filesystem::path& lookupPath);
    void Instantiate(
      Scene& scene,
      entt::entity e,
      entt::entity controller,
      std::function<std::shared_ptr<Texture>(int)> getTextTexture,
      std::function<std::shared_ptr<Texture>(int)> getScriptTexture) const;
  };

  struct AnimBlueprint {
    enum struct AnimMode {
      None,
      Trans,
      Rot
    } Mode = None;
    using enum AnimMode;
    int Parent = -1;
    glm::mat4 OffsetMatrix{ 1.f };
    float Offset = 0.f;
    float Delay = 0.f;
    float MaxSpeed = 0.f;
    float Factor = 0.f;
    int varIndex = -1;
  };

  struct LodBlueprint {
    float MinSize = 0.f;
    float MaxSize = std::numeric_limits<float>::max();
  };

  struct MeshBlueprint {
    std::shared_ptr<Mesh> Mesh = nullptr;
    int Lod = -1;
    int Viewpoint = 0;
    int Animation = -1;
    std::vector<MaterialBlueprint> Materials{};
    int VisibleVarIndex = -1;
    float VisibleCond = 0.f;
  };

  struct ModelBlueprint {

    void Instantiate(Scene& scene, entt::entity mapObject, entt::entity scriptObject) const;

    ModelBlueprint() = default;
    ModelBlueprint(
      const CfgFile& config,
      const std::filesystem::path& meshPath,
      const std::filesystem::path& texturePath,
      const std::function<int(std::string_view)>& varLookup,
      const std::function<int(std::string_view)>& stringVarLookup);

    glm::vec3 m_BoundsMax;
    glm::vec3 m_BoundsMin;

    std::vector<AnimBlueprint> Animations;
    std::vector<MeshBlueprint> Meshes;
    std::vector<LodBlueprint> Lods;

  };


}