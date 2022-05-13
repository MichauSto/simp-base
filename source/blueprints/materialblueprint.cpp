#include "materialblueprint.hpp"
#include "simp.hpp"
#include <regex>

namespace simp {

  MaterialBlueprint::TextureEntry MaterialBlueprint::ParseTexture(
    const std::string_view& name,
    const std::filesystem::path& lookupPath)
  {
    const static std::regex ttxRegex(R"__(^\\[Tt]\:(\d+)$)__", std::regex::ECMAScript);
    const static std::regex stxRegex(R"__(^\\[Ss]\:(\d+)$)__", std::regex::ECMAScript);

    std::cmatch sm;

    if (std::regex_match(name.data(), name.data() + name.size(), sm, ttxRegex, std::regex_constants::match_continuous)) {
      auto idx = sm[1].str();
      return TextTexture{ std::stoi(idx) };
    }
    else if (std::regex_match(name.data(), name.data() + name.size(), sm, stxRegex, std::regex_constants::match_continuous)) {
      auto idx = sm[1].str();
      return ScriptTexture{ std::stoi(idx) };
    }
    else {
      return Simp::GetTextureManager().Get(lookupPath / name);
    }
  }

  void MaterialBlueprint::Instantiate(
    Scene& scene,
    entt::entity e,
    entt::entity controller,
    std::function<std::shared_ptr<Texture>(int)> getTextTexture,
    std::function<std::shared_ptr<Texture>(int)> getScriptTexture) const
  {
    auto& matl = scene.GetRegistry().emplace<MaterialComponent>(e);
    matl.m_Material = Default.GetMaterial(controller, getTextTexture, getScriptTexture);
    if (varIndex >= 0 && Items.size()) {
      auto& matlChange = scene.GetRegistry().emplace<MaterialChangeComponent>(e);
      matlChange.Default = matl.m_Material;
      matlChange.Materials.reserve(Items.size());
      for (const auto& item : Items) {
        matlChange.Materials.emplace_back(
          item.GetMaterial(controller, getTextTexture, getScriptTexture));
      }
    }
  }


  Material MaterialBlueprint::Item::GetMaterial(entt::entity controller, std::function<std::shared_ptr<Texture>(int)> getTextTexture, std::function<std::shared_ptr<Texture>(int)> getScriptTexture) const
  {
    Material material{};
    for (int i = 0; i < Textures.size(); ++i) {
      std::shared_ptr<Texture> texture;
      if (std::holds_alternative<TextTexture>(Textures[i]))
        texture = getTextTexture ? getTextTexture(std::get<TextTexture>(Textures[i]).Index) : nullptr;
      else if (std::holds_alternative<ScriptTexture>(Textures[i]))
        texture = getScriptTexture ? getScriptTexture(std::get<ScriptTexture>(Textures[i]).Index) : nullptr;
      else
        texture = std::get<std::shared_ptr<Texture>>(Textures[i]);
      material.Textures[i] = texture ? texture->GetTextureView() : nullptr;
    }

    material.Data.Diffuse = Diffuse;
    material.Data.Alpha = Alpha;
    material.Data.Specular = Specular;
    material.Data.Shininess = Shininess;
    material.Data.Emissive = Emissive;
    material.Data.Ambient = Ambient;
    material.Data.EnvMapIntensity = EnvMapIntensity;
    material.Data.BumpMapIntensity = BumpMapIntensity;
    material.Data.LightMapIntensity = LightMapIntensity;

    material.BorderColor = BorderColor;
    material.WrapMode = WrapMode;
    material.AlphaMode = AlphaMode;

    material.ZbufCheckDisable = ZbufCheckDisable;
    material.ZbufWriteDisable = ZbufWriteDisable;

    material.CreateResources();
    return material;
  }

  bool MaterialBlueprint::Item::ParseConfig(
    const CfgFile& config, 
    const std::filesystem::path& texturePath)
  {
    if (config.TestTag("[matl_alpha]")) {
      AlphaMode = config.ReadInt();
      return true;
    }
    else if (config.TestTag("[matl_texadress_clamp]")) {
      WrapMode = D3D11_TEXTURE_ADDRESS_CLAMP;
      return true;
    }
    else if (config.TestTag("[matl_texadress_mirror]")) {
      WrapMode = D3D11_TEXTURE_ADDRESS_MIRROR;
      return true;
    }
    else if (config.TestTag("[matl_texadress_mirroronce]")) {
      WrapMode = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
      return true;
    }
    else if (config.TestTag("[matl_nozcheck]")) {
      ZbufCheckDisable = true;
      return true;
    }
    else if (config.TestTag("[matl_nozwrite]")) {
      ZbufWriteDisable = true;
      return true;
    }
    else if (config.TestTag("[matl_texadress_border]")) {
      WrapMode = D3D11_TEXTURE_ADDRESS_BORDER;
      config.ReadFloats((float*)&BorderColor, 4);
      BorderColor /= 255.f;
      return true;
    }
    else if (config.TestTag("[matl_envmap]")) {
      auto name = config.GetLine();
      auto intensity = config.ReadFloat();
      if (name.empty()) {
        // No filename given
        return true;
      }
      Textures[Material::Envmap] =
        MaterialBlueprint::ParseTexture(name, texturePath);
      EnvMapIntensity = intensity;
      return true;
    }
    else if (config.TestTag("[matl_bumpmap]")) {
      auto name = config.GetLine();
      auto intensity = config.ReadFloat();
      if (name.empty()) {
        // No filename given
        return true;
      }
      Textures[Material::Bumpmap] =
        MaterialBlueprint::ParseTexture(name, texturePath);
      BumpMapIntensity = intensity;
      return true;
    }
    else if (config.TestTag("[matl_envmap_mask]")) {
      auto name = config.GetLine();
      if (name.empty())
        Textures[Material::EnvmapMask] =
        Textures[Material::Diffuse];
      else Textures[Material::EnvmapMask] =
        MaterialBlueprint::ParseTexture(name, texturePath);
      return true;
    }
    else if (config.TestTag("[matl_transmap]")) {
      auto name = config.GetLine();
      if (name.empty())
        Textures[Material::Transmap] =
        Textures[Material::Diffuse];
      else Textures[Material::Transmap] =
        MaterialBlueprint::ParseTexture(name, texturePath);
      return true;
    }
    else if (config.TestTag("[matl_normalmap]")) {
      auto name = config.GetLine();
      if (name.empty()) {
        // No filename given
        return true;
      }
      else Textures[Material::Normalmap] =
        MaterialBlueprint::ParseTexture(name, texturePath);
      return true;
    }
    else if (config.TestTag("[matl_allcolor]")) {
      config.ReadFloats((float*)&Diffuse, 3);
      config.ReadFloats(&Alpha, 1);
      config.ReadFloats((float*)&Ambient, 3);
      config.ReadFloats((float*)&Specular, 3);
      config.ReadFloats((float*)&Emissive, 3);
      config.ReadFloats(&Shininess, 1);
      return true;
    }
    else if (config.TestTag("[usetexttexture]")) {
      auto Index = config.ReadInt();
      Textures[Material::Diffuse] = TextTexture(Index);
      return true;
    }
    else if (config.TestTag("[usescripttexture]")) {
      auto Index = config.ReadInt();
      Textures[Material::Diffuse] = ScriptTexture(Index);
      return true;
    }
    return false;
  }

}