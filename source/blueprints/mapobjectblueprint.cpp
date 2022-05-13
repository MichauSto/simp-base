#include "mapobjectblueprint.hpp"
#include "scene/transformcomponents.hpp"

namespace simp {

  entt::entity MapObjectBlueprint::Instantiate(Scene& scene, const glm::mat4& position, glm::ivec2 tile, int renderType) const
  {
    entt::entity root = scene.GetRegistry().create();
    scene.GetRegistry().emplace<TransformComponent>(root, position);
    scene.GetRegistry().emplace<TransformWorldComponent>(root, tile);

    ScriptBlueprint.Instantiate(scene, root);
    ModelBlueprint.Instantiate(scene, root, root, renderType);

    return root;
  }

}