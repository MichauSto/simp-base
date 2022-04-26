#include "loader.hpp"

#include <cassert>

using namespace simp::MeshLoaderApi;

O3dLoader::O3dLoader(const char* omsiDir)
{
}

PMeshInstance __stdcall O3dLoader::LoadMesh(uint64_t size, const char* data, const char* filename) const
{
  auto mesh = new O3dMesh();
  return mesh;
}

void __stdcall O3dLoader::FreeMesh(PMeshInstance instance) const
{
  auto mesh = static_cast<O3dMesh*>(instance);
  assert(mesh);
  delete mesh;
}

uint32_t __stdcall O3dLoader::GetMaterialCount(PMeshInstance instance) const
{
  auto mesh = static_cast<O3dMesh*>(instance);
  assert(mesh);

  return (uint32_t)mesh->Models.size();
}

const char* __stdcall O3dLoader::GetMaterialName(PMeshInstance instance, uint32_t materialId) const
{
  auto mesh = static_cast<O3dMesh*>(instance);
  assert(mesh);

  return mesh->Models[materialId].Material.Name.c_str();
}

void __stdcall O3dLoader::GetMaterialParams(
  simp::MeshLoaderApi::PMeshInstance instance,
  uint32_t materialId,
  float diffuse[],
  float ambient[],
  float specular[],
  float emit[],
  float& specularExp,
  float& alpha) const
{
  auto mesh = static_cast<O3dMesh*>(instance);
  assert(mesh);

  const auto& matl = mesh->Models[materialId].Material;
  *(glm::vec3*)diffuse = matl.Diffuse;
  *(glm::vec3*)ambient = { 1.f, 1.f, 1.f };
  *(glm::vec3*)specular = matl.Specular;
  *(glm::vec3*)emit = matl.Emissive;
  specularExp = matl.SpecularExp;
  alpha = matl.Alpha;
}

uint32_t __stdcall O3dLoader::GetTriangleCount(PMeshInstance instance, uint32_t materialId) const
{
  auto mesh = static_cast<O3dMesh*>(instance);
  assert(mesh);

  return (uint32_t)(mesh->Models[materialId].Indices.size() / 3);
}

void __stdcall O3dLoader::GetVertexPosition(PMeshInstance instance, uint32_t materialId, uint32_t triangleId, uint32_t vertexId, float position[]) const
{
  auto mesh = static_cast<O3dMesh*>(instance);
  assert(mesh);

  const auto& vertex = mesh->Vertices[mesh->Models[materialId].Indices[triangleId * 3 + vertexId]];
  *(glm::vec3*)position = vertex.Position;
}

void __stdcall O3dLoader::GetVertexNormal(PMeshInstance instance, uint32_t materialId, uint32_t triangleId, uint32_t vertexId, float normal[]) const
{
  auto mesh = static_cast<O3dMesh*>(instance);
  assert(mesh);

  const auto& vertex = mesh->Vertices[mesh->Models[materialId].Indices[triangleId * 3 + vertexId]];
  *(glm::vec3*)normal = vertex.Normal;
}

void __stdcall O3dLoader::GetVertexTexCoord(PMeshInstance instance, uint32_t materialId, uint32_t triangleId, uint32_t vertexId, float texCoord[]) const
{
  auto mesh = static_cast<O3dMesh*>(instance);
  assert(mesh);

  const auto& vertex = mesh->Vertices[mesh->Models[materialId].Indices[triangleId * 3 + vertexId]];
  *(glm::vec2*)texCoord = vertex.TexCoord;
}

void __stdcall O3dLoader::GetVertexIndices(PMeshInstance instance, uint32_t materialId, uint32_t triangleId, uint32_t vertexId, uint32_t bones[]) const
{
  auto mesh = static_cast<O3dMesh*>(instance);
  assert(mesh);
}

void __stdcall O3dLoader::GetVertexWeights(PMeshInstance instance, uint32_t materialId, uint32_t triangleId, uint32_t vertexId, float weights[]) const
{
  auto mesh = static_cast<O3dMesh*>(instance);
  assert(mesh);
}

uint32_t __stdcall O3dLoader::GetBoneCount(PMeshInstance instance) const
{
  auto mesh = static_cast<O3dMesh*>(instance);
  assert(mesh);
  return 0;
}

const char* __stdcall O3dLoader::GetBoneName(PMeshInstance instance, uint32_t boneId) const
{
  auto mesh = static_cast<O3dMesh*>(instance);
  assert(mesh);
  return nullptr;
}

void __stdcall O3dLoader::GetOriginTransform(PMeshInstance instance, float matrix[]) const
{
  auto mesh = static_cast<O3dMesh*>(instance);
  assert(mesh);

  *(glm::mat4*)matrix = mesh->Origin;
}
