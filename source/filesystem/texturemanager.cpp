#include "managers.hpp"
#include "file.hpp"
#include "utils/dict.hpp"
#include "simp.hpp"

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
    auto& cache = LocalCache[path.generic_string()];
    auto ptr = cache.lock();
    if (!ptr) cache = ptr = LoadTextureLocal(path);
    return ptr;
  }

  std::shared_ptr<Texture> TextureManager::LoadTextureLocal(const std::filesystem::path& path) const
  {
    auto systemPath = OmsiDir / path;
    if (!std::filesystem::is_regular_file(systemPath)) return nullptr;

    if (DictEqual(path.extension().generic_string(), ".tga")) {
      // Tga loader only
      return Texture::LoadTGA(Simp::GetOmsiPath() / path);
    }

    else if (DictEqual(path.extension().generic_string(), ".dds")) {
      // Try load dds
      return Texture::LoadDDS(Simp::GetOmsiPath() / path);
    }

    else {
      // Try WIC loader
      return Texture::LoadWIC(Simp::GetOmsiPath() / path);
    }
  }

}