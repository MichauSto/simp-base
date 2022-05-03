#include "scene.hpp"

namespace simp {

  void Scene::Update(float dt)
  {
    // UpdateVehicles(dt);
    
    // UpdatePhysicsWorld(dt);

    // PostUpdateVehicles(dt);

    UpdateScripts(dt);

    UpdateAnimations(dt);

    UpdateTransformHierarchy();

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