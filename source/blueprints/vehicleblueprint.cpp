#include "vehicleblueprint.hpp"

#include "simp.hpp"
#include "filesystem/file.hpp"
#include "filesystem/cfgfile.hpp"

namespace simp {

  VehicleBlueprint::VehicleBlueprint(const std::filesystem::path& path) {
    CfgFile config{ LoadFileText(Simp::GetOmsiPath() / path) };
    auto refPath = path.parent_path();

    std::filesystem::path modelPath = {};

    for (config.Reset(); !config.AtEnd(); config.SkipEmptyLines()) {
      if (config.TestTag("[model]")) {
        modelPath = refPath / config.GetLine();
      }
      else {
        config.NextLine();
      }
    }

    std::vector<std::filesystem::path> scriptPaths{};
    std::vector<std::filesystem::path> varlistPaths{ "program/varlist_roadvehicle.txt" };
    std::vector<std::filesystem::path> stringvarlistPaths{ "program/stringvarlist_roadvehicle.txt" };
    std::vector<std::filesystem::path> constfilePaths{};

    ScriptBlueprint::ReadScriptLists(
      config, 
      refPath, 
      scriptPaths, 
      varlistPaths, 
      stringvarlistPaths, 
      constfilePaths);

    ScriptBlueprint = {
      scriptPaths, 
      varlistPaths,
      stringvarlistPaths,
      constfilePaths };

    ModelBlueprint = {
      LoadFileText(Simp::GetOmsiPath() / modelPath),
      refPath / "Model",
      refPath / "Texture",
      std::bind(&ScriptBlueprint::getVarIndex, &ScriptBlueprint, std::placeholders::_1),
      std::bind(&ScriptBlueprint::getStringvarIndex, &ScriptBlueprint, std::placeholders::_1) };
  }

  entt::entity VehicleBlueprint::Instantiate(Scene& scene, const glm::mat4& position, glm::ivec2 tile) const
  {
    return MapObjectBlueprint::Instantiate(scene, position, tile, 10);
  }


}