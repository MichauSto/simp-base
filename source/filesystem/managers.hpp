#pragma once

#include "assetmgr.hpp"
#include "visual/mesh.hpp"
#include "visual/texture.hpp"

#include <memory>

namespace simp {

  struct MeshLoader;

  struct MeshManager : public AssetManager<Mesh> {
  public:
    MeshManager(const std::filesystem::path& _omsiDir);
  private:
    std::filesystem::path OmsiDir;
    std::shared_ptr<Mesh> LoadAsset(const std::filesystem::path& path) const override;
    std::shared_ptr<MeshLoader> Loader;
  };

  struct TextureManager : public AssetManager<Texture> {
  public:
    TextureManager(const std::filesystem::path& _omsiDir);
  private:
    std::filesystem::path OmsiDir;
    mutable Dict<std::weak_ptr<Texture>> LocalCache;
    std::shared_ptr<Texture> LoadAsset(const std::filesystem::path& path) const override;
    std::shared_ptr<Texture> GetTextureLocal(const std::filesystem::path& path) const;
    std::shared_ptr<Texture> LoadTextureLocal(const std::filesystem::path& path) const;

  };

}