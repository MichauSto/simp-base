#include "scene.hpp"

namespace simp {

  void Scene::Update(float dt)
  {
    // UpdateVehicles(dt);
    
    // UpdatePhysicsWorld(dt);

    // PostUpdateVehicles(dt);

    UpdateTransformHierarchy();

    UpdateScripts(dt);

    UpdateAnimations(dt);

    UpdateTransforms();

    // UpdateMaterials();

    // UpdateRenderers();

    ClearTransformFlags();
  }

  entt::registry& Scene::GetRegistry()
  {
    return Registry;
  }

}