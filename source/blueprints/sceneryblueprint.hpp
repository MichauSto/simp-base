#pragma once

#include "scene/scene.hpp"
#include "mapobjectblueprint.hpp"

#include <filesystem>

namespace simp {

  struct SceneryBlueprint : public MapObjectBlueprint
  {
    int RenderType = 8;
    bool Surface;
    std::shared_ptr<Mesh> CollisionMesh;
    bool AbsHeight = false;
    SceneryBlueprint() = default;
    SceneryBlueprint(const std::filesystem::path& path);
    entt::entity Instantiate(Scene& scene, const glm::mat4& position, glm::ivec2 tile) const;

  };

}