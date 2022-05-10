#include "vehicleblueprint.hpp"

#include "simp.hpp"
#include "filesystem/file.hpp"
#include "filesystem/cfgfile.hpp"
#include "scene/transformcomponents.hpp"

namespace simp {

  VehicleBlueprint::VehicleBlueprint(const std::filesystem::path& path) {
    CfgFile config{ LoadFileText(Simp::GetOmsiPath() / path) };
    auto refPath = path.parent_path();

    std::filesystem::path modelPath = {};
    std::vector<std::filesystem::path> scriptPaths{};
    std::vector<std::filesystem::path> varlistPaths{ "program/varlist_roadvehicle.txt" };
    std::vector<std::filesystem::path> stringvarlistPaths{ "program/stringvarlist_roadvehicle.txt"};
    std::vector<std::filesystem::path> constfilePaths{};

    for (config.Reset(); !config.AtEnd(); config.SkipEmptyLines()) {
      if (config.TestTag("[model]")) {
        modelPath = refPath / config.GetLine();
      }
      else if (config.TestTag("[script]")) {
        auto count = config.ReadInt();
        scriptPaths.reserve(scriptPaths.size() + count);
        for (; count > 0; --count)
          scriptPaths.emplace_back(refPath / config.GetLine());
      }
      else if (config.TestTag("[varnamelist]")) {
        auto count = config.ReadInt();
        varlistPaths.reserve(varlistPaths.size() + count);
        for (; count > 0; --count)
          varlistPaths.emplace_back(refPath / config.GetLine());
      }
      else if (config.TestTag("[stringvarnamelist]")) {
        auto count = config.ReadInt();
        stringvarlistPaths.reserve(stringvarlistPaths.size() + count);
        for (; count > 0; --count)
          stringvarlistPaths.emplace_back(refPath / config.GetLine());
      }
      else if (config.TestTag("[constfile]")) {
        auto count = config.ReadInt();
        constfilePaths.reserve(constfilePaths.size() + count);
        for (; count > 0; --count)
          constfilePaths.emplace_back(refPath / config.GetLine());
      }
      else {
        config.NextLine();
      }
    }

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

  entt::entity VehicleBlueprint::Instantiate(Scene& scene, const glm::mat4& position) const
  {
    entt::entity root = scene.GetRegistry().create();
    scene.GetRegistry().emplace<TransformComponent>(root, position);
    scene.GetRegistry().emplace<TransformWorldComponent>(root);

    ScriptBlueprint.Instantiate(scene, root);
    ModelBlueprint.Instantiate(scene, root, root);

    return root;
  }


}