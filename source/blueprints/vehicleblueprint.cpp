#include "vehicleblueprint.hpp"

namespace simp {



  entt::entity VehicleBlueprint::Instantiate(Scene& scene) const
  {
    entt::entity root = scene.GetRegistry().create();

    ScriptBlueprint::Instantiate(scene, root);
    ModelBlueprint::Instantiate(scene, root, root);
  }


}