#pragma once

#include "scene/scene.hpp"
#include "modelblueprint.hpp"
#include "scriptblueprint.hpp"

#include <filesystem>

namespace simp {

  struct MapObjectBlueprint
  {
    MapObjectBlueprint() = default;

    ModelBlueprint ModelBlueprint;
    ScriptBlueprint ScriptBlueprint;
    entt::entity Instantiate(Scene& scene, const glm::mat4& position, glm::ivec2 tile, int renderType) const;

  };

}