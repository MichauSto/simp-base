#pragma once

#include "utils/Windows.hpp"

#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace simp {

  struct Scene {
  public:
    Scene();
    void UpdateScripts(float dt);
    void UpdateAnimations(float dt);
    void UpdateTransformHierarchy();
    void UpdateTransforms();
    void UpdateRenderers(ID3D11DeviceContext* context);
    void UpdateMaterials(ID3D11DeviceContext* context);
    void UpdateVisible();
    void ClearTransformFlags();
    void UpdateReferenceTile();
    void Update(float dt);
    void Render(ID3D11DeviceContext* context, const glm::mat4& viewMatrix, const glm::vec3& eye);
    entt::registry& GetRegistry();
  private:
    entt::registry Registry;
    void OnTransformAdded(entt::registry& registry, entt::entity entity);
    void OnObjectDestroyed(entt::entity entity);
    glm::vec2 TileSize;
    glm::vec2 ReferenceTile;
    bool TransformHierarchyChanged;
  };

}