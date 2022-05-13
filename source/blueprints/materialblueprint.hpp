#pragma once

#include "filesystem/cfgfile.hpp"
#include "visual/texture.hpp"
#include "scene/materialcomponents.hpp"
#include <variant>

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
      int AlphaMode = 0;
      bool ZbufCheckDisable = false;
      bool ZbufWriteDisable = false;
      D3D11_TEXTURE_ADDRESS_MODE WrapMode = D3D11_TEXTURE_ADDRESS_WRAP;
      glm::vec4 BorderColor{};
      glm::vec3 Diffuse{ 1.f };
      float Alpha = 1.f;
      glm::vec3 Specular{ 0.f };
      float Shininess{ 1.f };
      glm::vec3 Emissive{ 0.f };
      glm::vec3 Ambient{ 1.f };
      float EnvMapIntensity{ 0.f };
      float BumpMapIntensity{ 0.f };
      float LightMapIntensity{ 0.f };
      bool ParseConfig(
        const CfgFile& config,
        const std::filesystem::path& texturePath);
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

}