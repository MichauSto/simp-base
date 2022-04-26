#include "loader.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cassert>

using namespace simp::MeshLoaderApi;

AiLoader::AiLoader(const char* omsiDir)
{
  Importer = std::make_unique<Assimp::Importer>();
  // Remove primitives other than triangles
  Importer->SetPropertyInteger(
    AI_CONFIG_PP_SBP_REMOVE,
    aiPrimitiveType_POINT |
    aiPrimitiveType_LINE |
    aiPrimitiveType_POLYGON);

  // Limit mesh size to 65535 verts
  Importer->SetPropertyInteger(
    AI_CONFIG_PP_SLM_VERTEX_LIMIT,
    0xffff);
}

PMeshInstance __stdcall AiLoader::LoadMesh(uint64_t size, const char* data, const char* filename) const
{
  auto scene = Importer->ReadFileFromMemory(data, size,
    aiProcessPreset_TargetRealtime_MaxQuality |
    aiProcess_FlipUVs);

  if (!scene) return nullptr;
  return new AiMesh(this, scene);
}

void __stdcall AiLoader::FreeMesh(PMeshInstance instance) const
{
  auto mesh = static_cast<AiMesh*>(instance);
  assert(mesh);

  delete mesh;
  Importer->FreeScene();
}

uint32_t __stdcall AiLoader::GetMaterialCount(PMeshInstance instance) const
{
  auto mesh = static_cast<AiMesh*>(instance);
  assert(mesh);

  return mesh->Scene->mNumMaterials;
}

const char* __stdcall AiLoader::GetMaterialName(PMeshInstance instance, uint32_t materialId) const
{
  auto mesh = static_cast<AiMesh*>(instance);
  assert(mesh);

  return mesh->MatNames[materialId].C_Str();
}

void __stdcall AiLoader::GetMaterialParams(PMeshInstance instance, uint32_t materialId, float diffuse[], float ambient[], float specular[], float emit[], float& specularExp, float& alpha) const
{
  auto mesh = static_cast<AiMesh*>(instance);
  assert(mesh);

  auto material = mesh->Scene->mMaterials[materialId];
  material->Get<aiColor3D>(AI_MATKEY_COLOR_DIFFUSE, *(aiColor3D*)diffuse);
  material->Get<aiColor3D>(AI_MATKEY_COLOR_AMBIENT, *(aiColor3D*)ambient);
  material->Get<aiColor3D>(AI_MATKEY_COLOR_SPECULAR, *(aiColor3D*)specular);
  material->Get<aiColor3D>(AI_MATKEY_COLOR_EMISSIVE, *(aiColor3D*)emit);
  material->Get<float>(AI_MATKEY_SHININESS, specularExp);
  material->Get<float>(AI_MATKEY_OPACITY, alpha);

}

uint32_t __stdcall AiLoader::GetTriangleCount(PMeshInstance instance, uint32_t materialId) const
{
  auto mesh = static_cast<AiMesh*>(instance);
  assert(mesh);
  return uint32_t();
}

void __stdcall AiLoader::GetVertexPosition(PMeshInstance instance, uint32_t materialId, uint32_t triangleId, uint32_t vertexId, float position[]) const
{
  auto mesh = static_cast<AiMesh*>(instance);
  assert(mesh);
}

void __stdcall AiLoader::GetVertexNormal(PMeshInstance instance, uint32_t materialId, uint32_t triangleId, uint32_t vertexId, float normal[]) const
{
  auto mesh = static_cast<AiMesh*>(instance);
  assert(mesh);
}

void __stdcall AiLoader::GetVertexTexCoord(PMeshInstance instance, uint32_t materialId, uint32_t triangleId, uint32_t vertexId, float texCoord[]) const
{
  auto mesh = static_cast<AiMesh*>(instance);
  assert(mesh);
}

void __stdcall AiLoader::GetVertexIndices(PMeshInstance instance, uint32_t materialId, uint32_t triangleId, uint32_t vertexId, uint32_t bones[]) const
{
  auto mesh = static_cast<AiMesh*>(instance);
  assert(mesh);
}

void __stdcall AiLoader::GetVertexWeights(PMeshInstance instance, uint32_t materialId, uint32_t triangleId, uint32_t vertexId, float weights[]) const
{
  auto mesh = static_cast<AiMesh*>(instance);
  assert(mesh);
}

uint32_t __stdcall AiLoader::GetBoneCount(PMeshInstance instance) const
{
  auto mesh = static_cast<AiMesh*>(instance);
  assert(mesh);
  return uint32_t();
}

const char* __stdcall AiLoader::GetBoneName(PMeshInstance instance, uint32_t boneId) const
{
  auto mesh = static_cast<AiMesh*>(instance);
  assert(mesh);
  return nullptr;
}

void __stdcall AiLoader::GetOriginTransform(PMeshInstance instance, float matrix[]) const
{
  auto mesh = static_cast<AiMesh*>(instance);
  assert(mesh);
}

AiMesh::AiMesh(const AiLoader* _loader, const aiScene* _scene)
  : Scene(_scene)
{
  MatNames.resize(Scene->mNumMaterials);
  for (int i = 0; i < MatNames.size(); ++i) {
    auto material = Scene->mMaterials[i];
    material->GetTexture(aiTextureType_DIFFUSE, 0, &MatNames[i]);
  }
}
