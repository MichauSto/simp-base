#pragma once

#include "scene/scene.hpp"
#include "modelblueprint.hpp"
#include "scriptblueprint.hpp"

#include <filesystem>

namespace simp {

  struct VehicleBlueprint
  {
    VehicleBlueprint() = default;
    VehicleBlueprint(const std::filesystem::path& path);

    ModelBlueprint ModelBlueprint;
    ScriptBlueprint ScriptBlueprint;
    entt::entity Instantiate(Scene& scene, const glm::mat4& position) const;

  };

}