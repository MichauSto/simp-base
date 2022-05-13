#pragma once

#include "assetmgr.hpp"
#include "visual/mesh.hpp"
#include "visual/texture.hpp"

#include "blueprints/sceneryblueprint.hpp"
#include "blueprints/vehicleblueprint.hpp"
#include "blueprints/splineblueprint.hpp"

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

  struct VehicleBlueprintManager : public AssetManager<VehicleBlueprint> {
  public:
    VehicleBlueprintManager() = default;
  private:
    std::shared_ptr<VehicleBlueprint> LoadAsset(const std::filesystem::path& path) const override;
  };

  struct SceneryBlueprintManager : public AssetManager<SceneryBlueprint> {
  public:
    SceneryBlueprintManager() = default;
  private:
    std::shared_ptr<SceneryBlueprint> LoadAsset(const std::filesystem::path& path) const override;
  };

  struct SplineBlueprintManager : public AssetManager<SplineBlueprint> {
  public:
    SplineBlueprintManager() = default;
  private:
    std::shared_ptr<SplineBlueprint> LoadAsset(const std::filesystem::path& path) const override;
  };

}