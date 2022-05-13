#include "managers.hpp"

namespace simp {

  std::shared_ptr<VehicleBlueprint> VehicleBlueprintManager::LoadAsset(const std::filesystem::path& path) const
  {
    return std::make_shared<VehicleBlueprint>(path);
  }
  
  std::shared_ptr<SceneryBlueprint> simp::SceneryBlueprintManager::LoadAsset(const std::filesystem::path& path) const
  {
    return std::make_shared<SceneryBlueprint>(path);
  }

  std::shared_ptr<SplineBlueprint> SplineBlueprintManager::LoadAsset(const std::filesystem::path& path) const
  {
    return std::make_shared<SplineBlueprint>(path);
  }

}