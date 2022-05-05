#pragma once

#include "scene/scene.hpp"
#include "modelblueprint.hpp"
#include "scriptblueprint.hpp"

#include <filesystem>

namespace simp {

  struct VehicleBlueprint : public ModelBlueprint, public ScriptBlueprint
  {
    VehicleBlueprint() = default;
    VehicleBlueprint(const std::filesystem::path& path);

    entt::entity Instantiate(Scene& scene) const;

  };

}