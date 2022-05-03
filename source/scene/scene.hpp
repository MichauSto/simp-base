#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace simp {

  struct Scene {
  public:
    void UpdateScripts(float dt);
    void UpdateAnimations(float dt);
    void UpdateTransformHierarchy();
    void UpdateTransforms();
    void ClearTransformFlags();
    void UpdateReferenceTile();
    void Update(float dt);
    entt::registry& GetRegistry();
  private:
    entt::registry Registry;
    glm::vec2 TileSize;
    glm::vec2 ReferenceTile;
    bool TransformHierarchyChanged;
  };

}