#pragma once

#include "simp/mesh/meshloader.hpp"

#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoord;
};

struct VertexWeights {
  glm::u32vec4 Indices;
  glm::vec4 Weights;
};

struct SubMesh {
  struct {
    glm::vec3 Diffuse;
    float Alpha;
    glm::vec3 Specular;
    glm::vec3 Emissive;
    float Shininess;
  } Material;
  std::string Name;
  std::vector<uint16_t> Indices;
};

struct Mesh {
  std::vector<Vertex> Vertices;
  std::vector<VertexWeights> Weights;
  std::vector<SubMesh> Models;
  glm::mat4 Origin;
  std::vector<std::string> Bones;
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
    float& shininess,
    float& alpha) const override;
  uint32_t __stdcall GetTriangleCount(
    simp::MeshLoaderApi::PMeshInstance instance, 
    uint32_t materialId) const override;
  void __stdcall GetVertex(
    simp::MeshLoaderApi::PMeshInstance instance,
    uint32_t materialId,
    uint32_t triangleId,
    uint32_t vertexId,
    float position[],
    float normal[],
    float texCoord[]) const override;
  void __stdcall GetVertexWeights(
    simp::MeshLoaderApi::PMeshInstance instance,
    uint32_t materialId,
    uint32_t triangleId,
    uint32_t vertexId,
    uint32_t bones[],
    float weights[]) const override;
  uint32_t __stdcall GetBoneCount(
    simp::MeshLoaderApi::PMeshInstance instance) const override;
  const char* __stdcall GetBoneName(
    simp::MeshLoaderApi::PMeshInstance instance, 
    uint32_t boneId) const override;
  void __stdcall GetOriginTransform(
    simp::MeshLoaderApi::PMeshInstance instance, 
    float matrix[]) const override;


};