#include "managers.hpp"
#include "meshloader.hpp"
#include "file.hpp"

namespace simp {

  MeshManager::MeshManager(const std::filesystem::path& _omsiDir)
    : OmsiDir(_omsiDir), Loader(std::make_shared<MeshLoader>(_omsiDir))
  {
  }

  std::shared_ptr<Mesh> MeshManager::LoadAsset(const std::filesystem::path& path) const
  {
    auto systemPath = OmsiDir / path;
    if (!std::filesystem::exists(systemPath)) return nullptr;
    return Loader->LoadMesh(LoadFileBinary(systemPath), path.generic_string());
  }

}