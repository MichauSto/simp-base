#include <plugins/mesh.hpp>
#include "loader.hpp"

using namespace simp::MeshLoaderApi;

IMeshLoader* InitMeshLoader(const char* omsiDir) {
  return new O3dLoader(omsiDir);
}

void FreeMeshLoader(IMeshLoader* loader) {
  delete loader;
}