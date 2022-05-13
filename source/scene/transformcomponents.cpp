#include "transformcomponents.hpp"
#include "scene.hpp"

#include <glm/gtx/transform.hpp>

namespace simp {

  void Scene::SetReferenceTile(glm::ivec2 tile)
  {
    if (tile == ReferenceTile) return;
    ReferenceTile = tile;
    for (auto e : Registry.view<TransformWorldComponent>()) {
      Registry.emplace_or_replace<TransformDirtyComponent>(e);
    }
  }

  void Scene::OnTransformAdded(entt::registry& registry, entt::entity entity)
  {
    TransformHierarchyChanged = true;
    Registry.emplace<TransformDirtyComponent>(entity);
  }

  void Scene::UpdateTransformHierarchy()
  {
    // Check whether there is a need for updating
    if (!TransformHierarchyChanged) return;
    TransformHierarchyChanged = false;

    // Assign missing transform hierarchy depths
    for (auto entity : Registry.view<TransformComponent>(
      entt::exclude<TransformDepthComponent>)) {
      auto& sort = Registry.emplace<TransformDepthComponent>(entity);
      sort.Depth = 0;
      for (; 
        Registry.any_of<TransformParentComponent>(entity); 
        entity = Registry.get<TransformParentComponent>(entity).Parent) {
        ++sort.Depth;
      }
    }

    // Sort transform hierarchy
    Registry.sort<TransformDepthComponent>(
      [](
        const TransformDepthComponent& lhs, 
        const TransformDepthComponent& rhs) -> bool 
      {
        return lhs.Depth < rhs.Depth;
      });
    Registry.sort<TransformComponent, TransformDepthComponent>();
  }

  void Scene::UpdateTransforms() {

    // Update top-level transforms
    {
      auto view = Registry.view<TransformComponent, TransformWorldComponent, TransformDirtyComponent>();
      for (const auto& entity : view) {
        auto [transform, world, dirty] = view.get<>(entity);
        transform.WorldTransform = 
          glm::translate(300.f * glm::vec3(world.Tile - ReferenceTile, 0.f)) * transform.LocalTransform;
      }
    }

    // Update child transforms
    {
      const auto& view = Registry.view<TransformComponent, TransformParentComponent>().use<TransformComponent>();
      for (const auto& entity : view) {
        auto [transform, parentHandle] = view.get<>(entity);
        auto parent = parentHandle.Parent;
        if (!Registry.any_of<TransformDirtyComponent>(entity) && !Registry.any_of<TransformDirtyComponent>(parent)) continue;
        Registry.emplace_or_replace<TransformDirtyComponent>(entity);
        transform.WorldTransform =
          Registry.get<TransformComponent>(parent).WorldTransform *
          transform.LocalTransform;
      }
    }
  }

  void Scene::ClearTransformFlags()
  {
    Registry.clear<TransformDirtyComponent>();
  }

}