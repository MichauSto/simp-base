#pragma once

#include <simp/mesh/meshloader.hpp>
#include <assimp/Importer.hpp>
#include <memory>
#include <vector>

struct AiLoader;

struct AiMesh {
  AiMesh(const AiLoader* loader, const aiScene* scene);
  const aiScene* Scene;
  std::vector<aiString> MatNames;
};

struct AiLoader : public simp::MeshLoaderApi::IMeshLoader {
  AiLoader(const char* omsiDir);
  ~AiLoader() = default;
  std::unique_ptr<Assimp::Importer> Importer;
  // Inherited via IMeshLoader
  simp::MeshLoaderApi::PMeshInstance __stdcall LoadMesh(
    uint64_t size,
    const char* data,
    const char* filename) const override;
  void __stdcall FreeMesh(
    simp::MeshLoaderApi::PMeshInstance instance) const override;
  uint32_t __stdcall GetMaterialCount(
    simp::MeshLoaderApi::PMeshInstance instance) const override;
  const char* __stdcall GetMaterialName(
    simp::MeshLoaderApi::PMeshInstance instance,
    uint32_t materialId) const override;
  void __stdcall GetMaterialParams(
    simp::MeshLoaderApi::PMeshInstance instance,
    uint32_t materialId,
    float diffuse[],
    float ambient[],
    float specular[],
    float emit[],
    float& specularExp,
    float& alpha) const override;
  uint32_t __stdcall GetTriangleCount(
    simp::MeshLoaderApi::PMeshInstance instance,
    uint32_t materialId) const override;
  void __stdcall GetVertexPosition(
    simp::MeshLoaderApi::PMeshInstance instance,
    uint32_t materialId,
    uint32_t triangleId,
    uint32_t vertexId,
    float position[]) const override;
  void __stdcall GetVertexNormal(
    simp::MeshLoaderApi::PMeshInstance instance,
    uint32_t materialId,
    uint32_t triangleId,
    uint32_t vertexId,
    float normal[]) const override;
  void __stdcall GetVertexTexCoord(
    simp::MeshLoaderApi::PMeshInstance instance,
    uint32_t materialId,
    uint32_t triangleId,
    uint32_t vertexId,
    float texCoord[]) const override;
  virtual void __stdcall GetVertexIndices(
    simp::MeshLoaderApi::PMeshInstance instance,
    uint32_t materialId,
    uint32_t triangleId,
    uint32_t vertexId,
    uint32_t bones[]) const override;
  virtual void __stdcall GetVertexWeights(
    simp::MeshLoaderApi::PMeshInstance instance,
    uint32_t materialId,
    uint32_t triangleId,
    uint32_t vertexId,
    float weights[]) const override;
  virtual uint32_t __stdcall GetBoneCount(
    simp::MeshLoaderApi::PMeshInstance instance) const override;
  virtual const char* __stdcall GetBoneName(
    simp::MeshLoaderApi::PMeshInstance instance,
    uint32_t boneId) const override;
  virtual void __stdcall GetOriginTransform(
    simp::MeshLoaderApi::PMeshInstance instance,
    float matrix[]) const override;

};