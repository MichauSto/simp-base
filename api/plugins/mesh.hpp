#pragma once
#include "simp/mesh/meshloader.hpp"

extern "C" {
  __declspec(dllexport) simp::MeshLoaderApi::IMeshLoader* __stdcall InitMeshLoader(const char* omsiDir);
  __declspec(dllexport) void __stdcall FreeMeshLoader(simp::MeshLoaderApi::IMeshLoader* loader);
}