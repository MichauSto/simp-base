#pragma once

#include "filesystem/managers.hpp"
#include "core/graphics.hpp"
#include "core/event.hpp"
#include "scene/scene.hpp"

#include <filesystem>

namespace simp {

  struct LaunchSettings {
    int argCallback(const std::string* arr, int size);
    std::filesystem::path omsiDir;
  };

  struct Simp {
  public:
    Simp(const LaunchSettings& settings);
    ~Simp();
    void Run(glm::mat4 position, glm::ivec2 tile);
  public:
    const static MeshManager& GetMeshManager();
    const static TextureManager& GetTextureManager();
    const static VehicleBlueprintManager& GetVehicleBlueprintManager();
    const static SceneryBlueprintManager& GetSceneryBlueprintManager();
    const static SplineBlueprintManager& GetSplineBlueprintManager();
    const static Graphics& GetGraphics();
    const static std::filesystem::path& GetOmsiPath();
    static Scene& GetScene();
  private:
    std::filesystem::path OmsiPath;
    void OnClose(const WindowCloseEvent& e);
    bool Running = true;
    entt::dispatcher Dispatcher;
    MeshManager MeshManager;
    TextureManager TextureManager;
    VehicleBlueprintManager VehicleBlueprintManager;
    SceneryBlueprintManager SceneryBlueprintManager;
    SplineBlueprintManager SplineBlueprintManager;
    Graphics Graphics;
    mutable Scene Scene;
  private:
    static const Simp* Instance;
  };

}