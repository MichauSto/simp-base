#pragma once

#include "simp/mesh/meshloader.hpp"

#include <glm/glm.hpp>
#include <string>
#include <vector>

struct O3dMaterial {
  glm::vec3 Diffuse;
  float Alpha;
  glm::vec3 Emissive;
  glm::vec3 Specular;
  float SpecularExp;
  std::string Name;
};

struct O3dVertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoord;
};

struct O3dSubMesh {
  O3dMaterial Material;
  std::vector<uint16_t> Indices;
};

struct O3dMesh {
  std::vector<O3dVertex> Vertices;
  std::vector<O3dSubMesh> Models;
  glm::mat4 Origin;
};

struct O3dLoader : public simp::MeshLoaderApi::IMeshLoader {
  O3dLoader(const char* omsiDir);
  ~O3dLoader() = default;
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