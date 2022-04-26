#pragma once

#include <Windows.h>
#include <vector>
#include <string>

namespace simp {

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
    MeshLoader(const std::string& omsiDir);
    void LoadImpl(const std::string& path, const std::string& omsiDir);
    ~MeshLoader();
    void LoadMesh(const std::string& data, const std::string& path) const;
  };


}