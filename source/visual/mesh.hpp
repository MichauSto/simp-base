#pragma once

#include "utils/Windows.hpp"

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>

#include <PxConfig.h>
#include <PxPhysicsAPI.h>

namespace simp {

  namespace MeshLoaderApi {
    struct IMeshLoader;
  }

  struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Tangent;
    float Sign;
    glm::vec2 TexCoord;
  };

  struct VertexWeights {
    glm::u32vec4 Indices;
    glm::vec4 Weights;
  };

  struct SubModel {
    std::string Name;
    uint32_t IndexOffset;
    uint32_t IndexCount;
    glm::vec3 Diffuse;
    glm::vec3 Ambient;
    glm::vec3 Specular;
    glm::vec3 Emissive;
    float Alpha;
    float Shininess;
  };

  struct Mesh {
  public:
    Mesh(
      std::vector<Vertex> _vertices, 
      std::vector<SubModel> _models,
      std::vector<std::string> _bones,
      std::vector<VertexWeights> _weights,
      const glm::mat4& _origin);
    ~Mesh();
    std::vector<Vertex> Vertices;
    std::vector<uint16_t> Indices;
    std::vector<SubModel> Models;
    glm::vec3 BoundsMin;
    glm::vec3 BoundsMax;
    glm::mat4 Origin;
    std::vector<std::string> Bones;
    std::vector<VertexWeights> Weights;
    physx::PxConvexMesh* GetConvexMesh();
    physx::PxTriangleMesh* GetTriangleMesh();
    physx::PxConvexMesh* ConvexMesh = nullptr;
    physx::PxTriangleMesh* TriangleMesh = nullptr;
    const SubModel& getModel(int index);
    int getModelCount() const;
    int getBoneIndex(const std::string_view& name) const;
    int getModelIndex(const std::string_view& name, int rpi) const;
    ComPtr<ID3D11Buffer> VertexBuffer;
    ComPtr<ID3D11Buffer> IndexBuffer;
  };

}