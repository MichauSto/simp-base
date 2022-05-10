#include "mesh.hpp"
#include "spatialsort/SpatialSort.hpp"
#include "utils/dict.hpp"
#include "simp.hpp"

#include <mikktspace.h>
#include <glm/gtc/type_ptr.hpp>

namespace simp {

  namespace {

    struct MikkContextHelper {
      std::vector<Vertex>& Vertices;
    };

    int getNumFaces(const SMikkTSpaceContext* context) {
      const MikkContextHelper* userData = (MikkContextHelper*)context->m_pUserData;
      return (int)(userData->Vertices.size() / 3);
    }

    int getNumVerticesOfFace(const SMikkTSpaceContext* context, const int iFace) {
      return 3;
    }

    void getPosition(const SMikkTSpaceContext* context, float* vertex, const int iFace, const int iVert) {
      MikkContextHelper* userData = (MikkContextHelper*)context->m_pUserData;
      *(glm::vec3*)vertex = userData->Vertices[iFace * 3 + iVert].Position;
    }

    void getNormal(const SMikkTSpaceContext* context, float* normal, const int iFace, const int iVert) {
      MikkContextHelper* userData = (MikkContextHelper*)context->m_pUserData;
      *(glm::vec3*)normal = userData->Vertices[iFace * 3 + iVert].Normal;
    }

    void getTexCoord(const SMikkTSpaceContext* context, float* uv, const int iFace, const int iVert) {
      MikkContextHelper* userData = (MikkContextHelper*)context->m_pUserData;
      *(glm::vec2*)uv = userData->Vertices[iFace * 3 + iVert].TexCoord;
    }

    void setTangentSpace(const SMikkTSpaceContext* context, const float* tangent, const float sign, const int iFace, const int iVert) {
      MikkContextHelper* userData = (MikkContextHelper*)context->m_pUserData;
      auto& vertex = userData->Vertices[iFace * 3 + iVert];
      vertex.Tangent = glm::make_vec3(tangent);
      vertex.Sign = sign;
    }

  }

  Mesh::Mesh(
    std::vector<Vertex> _vertices,
    std::vector<SubModel> _models,
    std::vector<std::string> _bones,
    std::vector<VertexWeights> _weights,
    const glm::mat4& _origin) : 
    Origin(_origin), 
    Bones(std::move(_bones)), 
    Models(std::move(_models)),
    BoundsMin(std::numeric_limits<float>::max()), 
    BoundsMax(-std::numeric_limits<float>::max())
  {
    // MTS user data
    MikkContextHelper helper{ _vertices };

    // MTS callback interface
    SMikkTSpaceInterface iMikkTS{};
    iMikkTS.m_getNumFaces = &getNumFaces;
    iMikkTS.m_getNumVerticesOfFace = &getNumVerticesOfFace;
    iMikkTS.m_getPosition = &getPosition;
    iMikkTS.m_getNormal = &getNormal;
    iMikkTS.m_getTexCoord = &getTexCoord;
    iMikkTS.m_setTSpaceBasic = &setTangentSpace;

    // MTS context
    SMikkTSpaceContext context{};
    context.m_pUserData = &helper;
    context.m_pInterface = &iMikkTS;

    // Create tangent space
    genTangSpaceDefault(&context);

    // Initialize spatial sort
    SpatialSort sort((const char*)_vertices.data(), _vertices.size(), sizeof(Vertex), offsetof(Vertex, Vertex::Position));

    // Helper flag to speed up the comparison
    bool hasBones = Bones.size();

    // Allocate index buffer
    Indices.resize(_vertices.size(), 0xffff);
    for (int i = 0; i < Indices.size(); ++i) {
      // Skip if index already assigned
      if (Indices[i] < 0xffff) continue;

      // Create new vertex
      Indices[i] = (uint16_t)Vertices.size();
      const auto& vertex = Vertices.emplace_back(_vertices[i]);
      const VertexWeights* weights = hasBones ? &Weights.emplace_back(_weights[i]) : nullptr;

      // Update mesh bounds
      BoundsMax = glm::max(BoundsMax, vertex.Position);
      BoundsMin = glm::min(BoundsMin, vertex.Position);

      // Find and optimize away identical vertices
      sort.FindPositions(vertex.Position, 1e-5f, [&](const int index) -> bool {
        if ((index == i) || Indices[index] < 0xffff) return false;
        const auto& rhs = _vertices[index];
        if (
          (vertex.Sign != rhs.Sign) ||
          (glm::dot(vertex.Normal, rhs.Normal) < 0.99984769515f) || // = difference is not greater than 1deg
          (glm::dot(vertex.Tangent, rhs.Tangent) < 0.99984769515f) ||
          glm::any(glm::epsilonNotEqual(vertex.TexCoord, rhs.TexCoord, 1e-6f))) return false;
        if (weights) {
          const auto& rhs = _weights[index];
          if ((weights->Indices != rhs.Indices) ||
            glm::any(glm::epsilonNotEqual(weights->Weights, rhs.Weights, 1e-3f))) return false;
        }
        Indices[index] = Indices[i];
        return false;
        });
    }

    D3D11_BUFFER_DESC vbDesc{};
    vbDesc.ByteWidth = Vertices.size() * sizeof(Vertices[0]);
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = 0;
    vbDesc.MiscFlags = 0;
    vbDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vbData{};
    vbData.pSysMem = Vertices.data();

    Simp::GetGraphics().GetDevice()->CreateBuffer(&vbDesc, &vbData, &VertexBuffer);

    D3D11_BUFFER_DESC ibDesc{};
    ibDesc.ByteWidth = Indices.size() * sizeof(Indices[0]);
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = 0;
    ibDesc.MiscFlags = 0;
    ibDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA ibData{};
    ibData.pSysMem = Indices.data();

    Simp::GetGraphics().GetDevice()->CreateBuffer(&ibDesc, &ibData, &IndexBuffer);
  }

  const SubModel& Mesh::getModel(int index)
  {
    return Models[index];
  }

  int Mesh::getModelCount() const
  {
    return (int)Models.size();
  }

  int Mesh::getBoneIndex(const std::string_view& name) const
  {
    for (int i = 0; i < Bones.size(); ++i) {
      if (!DictEqual(name, Bones[i])) continue;
      return i;
    }
    return -1;
  }

  int Mesh::getModelIndex(const std::string_view& name, int rpi) const
  {
    int idx = 0;
    for (int i = 0; i < Models.size(); ++i) {
      const auto& model = Models[i];
      if (!DictEqual(name, model.Name)) continue;
      if (rpi == idx++) return i;
    }
    return -1;
  }


}