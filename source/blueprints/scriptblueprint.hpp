#pragma once

#include "scene/scene.hpp"

#include <vector>
#include <filesystem>

namespace simp {

  struct ScriptBlueprint {

    ScriptBlueprint() = default;
    ScriptBlueprint(
      const std::vector<std::filesystem::path>& scriptFiles,
      const std::vector<std::filesystem::path>& varlistFiles,
      const std::vector<std::filesystem::path>& stringVarlistFiles,
      const std::vector<std::filesystem::path>& constFiles);

    void Instantiate(Scene& scene, entt::entity object) const;

  };

}