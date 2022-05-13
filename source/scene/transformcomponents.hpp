#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace simp {

  struct TransformComponent {
    glm::mat4 LocalTransform;
    glm::mat4 WorldTransform;
  };

  struct TransformDepthComponent {
    int Depth;
  };

  struct TransformDirtyComponent {
    int Dummy;
  };

  struct TransformWorldComponent {
    glm::ivec2 Tile;
  };

  struct TransformParentComponent {
    entt::entity Parent;
  };


}