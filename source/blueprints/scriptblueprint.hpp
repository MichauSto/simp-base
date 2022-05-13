#pragma once

#include "scene/scene.hpp"
#include "utils/dict.hpp"
#include "filesystem/cfgfile.hpp"

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

    static void ReadScriptLists(
      const CfgFile& config,
      const std::filesystem::path& refPath,
      std::vector<std::filesystem::path>& scriptFiles,
      std::vector<std::filesystem::path>& varlistFiles,
      std::vector<std::filesystem::path>& stringVarlistFiles,
      std::vector<std::filesystem::path>& constFiles);

    int getVarIndex(const std::string_view& key) const;
    int getStringvarIndex(const std::string_view& key) const;

    void Instantiate(Scene& scene, entt::entity object) const;
    Dict<int> varLookup{};
    Dict<int> stringVarLookup{};

    int varCount = 0;
    int stringVarCount = 0;
  };

}