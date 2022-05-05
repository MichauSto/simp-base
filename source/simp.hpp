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
    void Run();
  public:
    const static MeshManager& GetMeshManager();
    const static TextureManager& GetTextureManager();
    const static Graphics& GetGraphics();
    static Scene& GetScene();
  private:
    void OnClose(const WindowCloseEvent& e);
    bool Running = true;
    entt::dispatcher Dispatcher;
    MeshManager MeshManager;
    TextureManager TextureManager;
    Graphics Graphics;
    mutable Scene Scene;
  private:
    static const Simp* Instance;
  };

}