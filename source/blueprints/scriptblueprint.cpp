#include "scriptblueprint.hpp"
#include "scene/scriptcomponents.hpp"
#include "simp.hpp"
#include "filesystem/file.hpp"
#include "filesystem/cfgfile.hpp"

namespace simp {



  ScriptBlueprint::ScriptBlueprint(const std::vector<std::filesystem::path>& scriptFiles, const std::vector<std::filesystem::path>& varlistFiles, const std::vector<std::filesystem::path>& stringVarlistFiles, const std::vector<std::filesystem::path>& constFiles)
  {
    for (const auto& path : varlistFiles) {
      CfgFile varlist{ LoadFileText(Simp::GetOmsiPath() / path) };
      for (varlist.Reset(); !varlist.AtEnd(); varlist.SkipEmptyLines()) {
        if (varlist.LineEmpty()) continue;
        auto name = varlist.GetLine();
        if (varLookup.contains(name)) continue;
        varLookup.emplace(name, varCount++);
      }
    }
    for (const auto& path : stringVarlistFiles) {
      CfgFile varlist{ LoadFileText(Simp::GetOmsiPath() / path) };
      for (varlist.Reset(); !varlist.AtEnd(); varlist.SkipEmptyLines()) {
        if (varlist.LineEmpty()) continue;
        auto name = varlist.GetLine();
        if (stringVarLookup.contains(name)) continue;
        stringVarLookup.emplace(name, stringVarCount++);
      }
    }
  }

  void ScriptBlueprint::ReadScriptLists(
    const CfgFile& config,
    const std::filesystem::path& refPath,
    std::vector<std::filesystem::path>& scriptFiles, 
    std::vector<std::filesystem::path>& varlistFiles, 
    std::vector<std::filesystem::path>& stringVarlistFiles,
    std::vector<std::filesystem::path>& constFiles)
  {

    for (config.Reset(); !config.AtEnd(); config.SkipEmptyLines()) {
      if (config.TestTag("[script]")) {
        auto count = config.ReadInt();
        scriptFiles.reserve(scriptFiles.size() + count);
        for (; count > 0; --count)
          scriptFiles.emplace_back(refPath / config.GetLine());
      }
      else if (config.TestTag("[varnamelist]")) {
        auto count = config.ReadInt();
        varlistFiles.reserve(varlistFiles.size() + count);
        for (; count > 0; --count)
          varlistFiles.emplace_back(refPath / config.GetLine());
      }
      else if (config.TestTag("[stringvarnamelist]")) {
        auto count = config.ReadInt();
        stringVarlistFiles.reserve(stringVarlistFiles.size() + count);
        for (; count > 0; --count)
          stringVarlistFiles.emplace_back(refPath / config.GetLine());
      }
      else if (config.TestTag("[constfile]")) {
        auto count = config.ReadInt();
        constFiles.reserve(constFiles.size() + count);
        for (; count > 0; --count)
          constFiles.emplace_back(refPath / config.GetLine());
      }
      else {
        config.NextLine();
      }
    }
  }

  int ScriptBlueprint::getVarIndex(const std::string_view& key) const
  {
    auto it = varLookup.find(key);
    if (it != varLookup.end()) return (*it).second;
    return -1;
  }

  int ScriptBlueprint::getStringvarIndex(const std::string_view& key) const
  {
    auto it = stringVarLookup.find(key);
    if (it != stringVarLookup.end()) return (*it).second;
    return -1;
  }

  void ScriptBlueprint::Instantiate(Scene& scene, entt::entity object) const
  {
    auto& comp = scene.GetRegistry().emplace<ScriptComponent>(object);
    comp.VarList = std::vector<float>(varCount, 0.f);
  }


}