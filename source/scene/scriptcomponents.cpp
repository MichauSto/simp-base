#include "scene.hpp"
#include "scriptcomponents.hpp"
#include "transformcomponents.hpp"

namespace simp {

    Scene::Scene()
    {
      Registry.on_construct<TransformComponent>()
        .connect<&Scene::OnTransformAdded>(this);
    }

    void Scene::UpdateScripts(float dt) {
    
    }

}