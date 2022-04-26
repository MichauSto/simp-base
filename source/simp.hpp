#pragma once

#include "filesystem/managers.hpp"

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
    void run();
  public:
    const static MeshManager& GetMeshManager();
    const static TextureManager& GetTextureManager();
  private:
    MeshManager MeshManager;
    TextureManager TextureManager;
  private:
    static const Simp* Instance;
  };

}