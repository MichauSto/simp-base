#pragma once

#include "scene/scene.hpp"
#include "mapobjectblueprint.hpp"

#include <filesystem>

namespace simp {

  struct VehicleBlueprint : public MapObjectBlueprint
  {
    VehicleBlueprint() = default;
    VehicleBlueprint(const std::filesystem::path& path);
    entt::entity Instantiate(Scene& scene, const glm::mat4& position, glm::ivec2 tile) const;

  };

}