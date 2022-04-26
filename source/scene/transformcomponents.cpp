#include "transformcomponents.hpp"
#include "scene.hpp"

#include <glm/gtx/transform.hpp>

namespace simp {

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
      auto view = Registry.view<TransformComponent, TransformWorldComponent>();
      for (const auto& entity : view) {
        auto [transform, world] = view.get<>(entity);
        transform.WorldTransform = transform.LocalTransform;
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

  void Scene::UpdateReferenceTile() {
    auto view = Registry.view<TransformComponent, TransformWorldComponent>();
    for (const auto& entity : view) {
      auto [transform, world] = view.get<>(entity);
      transform.WorldTransform = 
        glm::translate(
          glm::vec3(
            TileSize * (world.Tile - ReferenceTile), 
            0.f)) * 
        transform.LocalTransform;

      Registry.emplace_or_replace<TransformDirtyComponent>(entity);
    }
  }

}