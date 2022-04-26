#pragma once

#include <Windows.h>
#include <vector>
#include <string>
#include <memory>
#include <filesystem>

namespace simp {

  struct Mesh;

  namespace MeshLoaderApi {
    struct IMeshLoader;
  }

  struct MeshLoaderImpl {
    HMODULE Module;
    MeshLoaderApi::IMeshLoader* Loader;
    MeshLoaderImpl(HMODULE _module, MeshLoaderApi::IMeshLoader* _loader)
      : Module(_module), Loader(_loader) {};
  };

  struct MeshLoader {
    std::vector<MeshLoaderImpl> Loaders;
    MeshLoader(const std::filesystem::path& omsiDir);
    void LoadImpl(const std::string& path, const std::string& omsiDir);
    ~MeshLoader();
    std::shared_ptr<Mesh> LoadMesh(const std::string& data, const std::string& path) const;
  };


}