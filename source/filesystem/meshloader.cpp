#include "meshloader.hpp"
#include "simp/mesh/meshloader.hpp"
#include <filesystem>

using namespace simp;

namespace {
  typedef MeshLoaderApi::IMeshLoader* (__stdcall* InitMeshLoader)(const char*);
  typedef void(__stdcall* FreeMeshLoader)(MeshLoaderApi::IMeshLoader*);
}

MeshLoader::MeshLoader(const std::string& omsiDir)
{
  for (const auto& entry : std::filesystem::recursive_directory_iterator(std::filesystem::current_path() / "plugins/mesh")) {
    if (entry.path().extension() != ".dll") continue;
    LoadImpl(entry.path().generic_string(), omsiDir);
  }
}

void MeshLoader::LoadImpl(const std::string& path, const std::string& omsiDir)
{
  auto module = LoadLibrary(path.c_str());
  if (!module) {
    return;
  }
  auto iml = (InitMeshLoader)GetProcAddress(module, "InitMeshLoader");
  if (!iml) {
    FreeLibrary(module);
    return;
  }
  auto loader = iml(omsiDir.c_str());
  if (!loader) {
    FreeLibrary(module);
    return;
  }
  Loaders.emplace_back(module, loader);
}

MeshLoader::~MeshLoader()
{
  for (const auto& impl : Loaders) {
    auto fml = (FreeMeshLoader)GetProcAddress(impl.Module, "FreeMeshLoader");
    if (fml) {
      fml(impl.Loader);
    }
    else {

    }
    FreeModule(impl.Module);
  }
}

void MeshLoader::LoadMesh(const std::string& data, const std::string& path) const
{
  for (const auto& [module, loader] : Loaders) {
    auto mesh = loader->LoadMesh(data.size(), data.data(), path.c_str());
    if (!mesh) continue;



    loader->FreeMesh(mesh);
  }
}
