#include "scene.hpp"
#include "transformcomponents.hpp"
#include "rendercomponents.hpp"
#include "simp.hpp"

namespace simp {

  Scene::Scene()
  {
    Registry.on_construct<TransformComponent>()
      .connect<&Scene::OnTransformAdded>(this);
  }

  void Scene::Update(float dt)
  {
    // UpdateVehicles(dt);
    
    // UpdatePhysicsWorld(dt);

    // PostUpdateVehicles(dt);

    UpdateTransformHierarchy();

    UpdateScripts(dt);

    UpdateAnimations(dt);

    UpdateTransforms();

    UpdateVisible();

    UpdateMaterials(Simp::GetGraphics().GetImmediateContext());

    UpdateRenderers(Simp::GetGraphics().GetImmediateContext());

    ClearTransformFlags();
  }

  entt::registry& Scene::GetRegistry()
  {
    return Registry;
  }

}