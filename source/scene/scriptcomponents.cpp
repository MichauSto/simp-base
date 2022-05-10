#include "scene.hpp"
#include "scriptcomponents.hpp"
#include "transformcomponents.hpp"
#include "rendercomponents.hpp"

namespace simp {

  void Scene::UpdateVisible()
  {
    // (1) Sync animations with script
    {
      auto view = Registry.view<VisibleComponent>();
      for (const auto& entity : view) {
        auto [vis] = view.get<>(entity);
        const auto& source = Registry.get<ScriptComponent>(vis.Controller);
        vis.Visible = source.VarList[vis.VarIndex] == vis.Condition;
      }
    }
  }

  void Scene::UpdateScripts(float dt) {
    
  }

}