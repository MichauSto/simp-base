#include "managers.hpp"
#include "file.hpp"

#include <DirectXTex.h>

namespace simp {

  TextureManager::TextureManager(const std::filesystem::path& _omsiDir)
    : OmsiDir(_omsiDir)
  {
  }

  std::shared_ptr<Texture> TextureManager::LoadAsset(const std::filesystem::path& path) const
  {
    if (path.extension() != ".dds") {
      auto ddsPath = path;
      auto result = GetTextureLocal(ddsPath.replace_extension(".dds"));
      if (result) return result;
    }
    auto result = GetTextureLocal(path);
    if (result) return result;

    static const std::filesystem::path texRoot = "Texture";

    if (!IsParentPath(texRoot, path)) {
      return Get(std::filesystem::path(texRoot / path.filename()));
    }

    // TODO return default placeholder
    return nullptr;
  }

  std::shared_ptr<Texture> TextureManager::GetTextureLocal(const std::filesystem::path& path) const
  {
    auto systemPath = OmsiDir / path;
    if (!std::filesystem::is_regular_file(systemPath)) return nullptr;

    auto data = LoadFileBinary(systemPath);

    if (path.extension() == ".tga") {
      // Tga loader only
      auto result = Texture::LoadTGA(data);
      return result;
    }

    if (path.extension() == ".dds") {
      // Try load dds
      auto result = Texture::LoadDDS(data);
    }

    // Try WIC loader
    return Texture::LoadWIC(data);
  }

}