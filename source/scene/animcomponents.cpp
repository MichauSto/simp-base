#include "scene.hpp"
#include "animcomponents.hpp"
#include "scriptcomponents.hpp"
#include "transformcomponents.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace simp {

  void Scene::UpdateAnimations(float dt)
  {
    // (1) Sync animations with script
    {
      auto view = Registry.view<AnimComponent>();
      for (const auto& entity : view) {
        auto [anim] = view.get<>(entity);
        const auto& source = Registry.get<ScriptComponent>(anim.VarSource);
        anim.PrevVar = std::exchange(
          anim.Var,
          source.VarList[anim.VarIndex]);
      }
    }

    // (2) Apply exponential delay
    {
      auto view = Registry.view<
        AnimComponent,
        AnimDelayComponent>();
      for (const auto& entity : view) {
        auto [anim, delay] = view.get<>(entity);
        anim.Var = anim.PrevVar + (anim.Var - anim.PrevVar) * delay.Delay * dt;
      }
    }

    // (3) Apply maxspeed
    {
      auto view = Registry.view<
        AnimComponent,
        AnimSpeedComponent>();
      for (const auto& entity : view) {
        auto [anim, speed] = view.get<>(entity);
        anim.Var = glm::clamp(
          anim.Var,
          anim.PrevVar - speed.MaxSpeed * dt,
          anim.PrevVar + speed.MaxSpeed * dt);
      }
    }

    // (4) Mark animations that have been updated
    {
      auto view = Registry.view<AnimComponent>();
      // TODO After LOD system is implemented, omit syncing animations outside view range
      for (const auto& entity : view) {
        auto [anim] = view.get<>(entity);
        if (anim.Var != anim.CachedVar) {
          anim.CachedVar = anim.Var;
          Registry.emplace_or_replace<AnimDirtyComponent>(entity);
          Registry.emplace_or_replace<TransformDirtyComponent>(entity);
        }
      }
    }

    // (5a) Calculate translation transforms
    {
      auto view = Registry.view<
        AnimComponent,
        TransformComponent,
        AnimDirtyComponent,
        AnimTransComponent>();
      for (const auto& entity : view) {
        auto [anim, transform, dirty, trans] = view.get<>(entity);
        transform.LocalTransform =
          anim.WorldMatrix *
          glm::translate(glm::vec3{ anim.Var * anim.Factor + anim.Offset, 0.f, 0.f });
        anim.InverseWorldMatrix;
      }
    }

    // (5b) Calculate rotation transforms
    {
      auto view = Registry.view<
        AnimComponent,
        TransformComponent,
        AnimDirtyComponent,
        AnimRotComponent>();
      for (const auto& entity : view) {
        auto [anim, transform, dirty, rot] = view.get<>(entity);
        transform.LocalTransform =
          anim.WorldMatrix *
          glm::eulerAngleX(anim.Var * glm::radians(anim.Factor) + glm::radians(anim.Offset)) *
          anim.InverseWorldMatrix;
      }
    }

    // (6) Clean-up flags
    {
      Registry.clear<AnimDirtyComponent>();
    }
  }

}