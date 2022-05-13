#include "sceneryblueprint.hpp"

#include "simp.hpp"
#include "filesystem/file.hpp"
#include "filesystem/cfgfile.hpp"

namespace simp {



  SceneryBlueprint::SceneryBlueprint(const std::filesystem::path& path)
  {
    CfgFile config{ LoadFileText(Simp::GetOmsiPath() / path) };
    auto refPath = path.parent_path();

    for (config.Reset(); !config.AtEnd(); config.SkipEmptyLines()) {
      if (config.TestTag("[rendertype]")) {
        const static Dict<int> renderTypes{
          { "presurface", 1 },
          { "surface", 3 },
          { "on_surface", 5 },
          { "1", 7 },
          { "2", 8 },
          { "3", 9 },
          { "4", 10 }
        };
        auto p = renderTypes.find(config.GetLine());
        if (p == renderTypes.end()) {
          // Invalid render type
          continue;
        }
        RenderType = (*p).second;
      }
      else if (config.TestTag("[surface]")) {
        Surface = true;
      }
      else if (config.TestTag("[collision_mesh]")) {
        CollisionMesh = Simp::GetMeshManager().Get(refPath / "model" / config.GetLine());
      }
      else if (config.TestTag("[absheight]")) {
        AbsHeight = true;
      }
      else {
        config.NextLine();
      }
    }

    std::vector<std::filesystem::path> scriptPaths{};
    std::vector<std::filesystem::path> varlistPaths{ "program/varlist_scenobj.txt" };
    std::vector<std::filesystem::path> stringvarlistPaths{};
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
      config,
      refPath / "Model",
      refPath / "Texture",
      std::bind(&ScriptBlueprint::getVarIndex, &ScriptBlueprint, std::placeholders::_1),
      std::bind(&ScriptBlueprint::getStringvarIndex, &ScriptBlueprint, std::placeholders::_1) };
  }

  entt::entity SceneryBlueprint::Instantiate(Scene& scene, const glm::mat4& position, glm::ivec2 tile) const
  {
    auto root = MapObjectBlueprint::Instantiate(scene, position, tile, RenderType);

    if (Surface && CollisionMesh) {
      scene.GetRegistry().emplace<StaticComponent>(root, CollisionMesh);
    }

    return root;
  }


}