#include "simp.hpp"

namespace simp {

  int LaunchSettings::argCallback(const std::string* arr, int size)
  {
    if (!_stricmp(arr[0].c_str(), "-omsi")) {
      omsiDir = arr[1];
      return 2;
    }
    return 1;
  }

  Simp::Simp(const LaunchSettings& settings)
    : MeshManager(settings.omsiDir), TextureManager(settings.omsiDir)
  {
    assert(!Instance);
    Instance = this;
  }

  Simp::~Simp()
  {
    assert(Instance == this);
    Instance = nullptr;
  }

  const MeshManager& Simp::GetMeshManager()
  {
    assert(Instance);
    return Instance->MeshManager;
  }

  const TextureManager& Simp::GetTextureManager()
  {
    assert(Instance);
    return Instance->TextureManager;
  }

  const Simp* Simp::Instance;

}
