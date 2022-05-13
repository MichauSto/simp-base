#include "scene.hpp"
#include "transformcomponents.hpp"
#include "rendercomponents.hpp"
#include "simp.hpp"

namespace simp {

  Scene::Scene()
  {


    Registry.on_construct<TransformComponent>()
      .connect<&Scene::OnTransformAdded>(this);

    Registry.on_construct<StaticComponent>()
      .connect<&Scene::OnStaticAdded>(this);

    Registry.on_destroy<StaticComponent>()
      .connect<&Scene::OnStaticDestroyed>(this);
  }

  const PhysicsWorld& Scene::GetPhysicsWorld() const
  {
    return PhysicsWorld;
  }

  void Scene::Update(float dt)
  {
    // UpdateVehicles(dt);
    
    UpdatePhysicsWorld(dt);

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