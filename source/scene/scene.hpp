#pragma once

#include "utils/Windows.hpp"
#include "physicscomponents.hpp"

#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include <PxConfig.h>
#include <PxPhysicsAPI.h>

namespace simp {

  struct Scene {
  public:
    Scene();
    void UpdatePhysicsWorld(float dt);
    void UpdateScripts(float dt);
    void UpdateAnimations(float dt);
    void UpdateTransformHierarchy();
    void UpdateTransforms();
    void UpdateRenderers(ID3D11DeviceContext* context);
    void UpdateMaterials(ID3D11DeviceContext* context);
    void UpdateVisible();
    void ClearTransformFlags();
    void Update(float dt);
    void Render(ID3D11DeviceContext* context, const glm::mat4& viewMatrix, const glm::vec3& eye);
    entt::registry& GetRegistry();
    void SetReferenceTile(glm::ivec2 tile);
    const PhysicsWorld& GetPhysicsWorld() const;
  private:
    PhysicsWorld PhysicsWorld{};
    entt::registry Registry;
    void OnTransformAdded(entt::registry& registry, entt::entity entity);
    void OnObjectDestroyed(entt::entity entity);
    void OnStaticAdded(entt::registry& registry, entt::entity entity);
    void OnStaticDestroyed(entt::registry& registry, entt::entity entity);
    glm::vec2 TileSize;
    glm::ivec2 ReferenceTile;
    bool TransformHierarchyChanged;
  };

}