#include "loader.hpp"

#include <utils/CP1252_to_UTF8.hpp>
#include <cassert>

#include <glm/gtx/compatibility.hpp>

using namespace simp::MeshLoaderApi;

O3dLoader::O3dLoader(const char* omsiDir)
{

}

PMeshInstance O3dLoader::LoadMesh(uint64_t size, const char* data, const char* filename) const
{
	static const char magic[3] = { '\x84' , '\x19', '\x01' };
	if (memcmp(data, magic, 3)) {
		// TODO fail
		return nullptr;
	}
	size_t ctr = 3;
	char tok;

	struct Triangle {
		uint16_t indices[3];
		uint16_t material;
	};
#pragma pack(push, 1)
	struct Bone {
		uint16_t vertex;
		float weight;
	};
#pragma pack(pop)

	auto mesh = new Mesh();
	Triangle* indBuf;
	int triCount = 0;

	std::vector<std::tuple<Bone*, uint16_t>> weights{};

	while (ctr < size) {
		tok = data[ctr++];
		switch (tok) {
			case '\x17':
			{
				uint16_t count = *(uint16_t*)&data[ctr];
				ctr += sizeof(uint16_t);
				auto size = count * sizeof(Vertex);
				mesh->Vertices.resize(count);
				memcpy(mesh->Vertices.data(), &data[ctr], size);
				ctr += size;
				break;
			}
			case '\x49':
			{
				triCount = *(uint16_t*)&data[ctr];
				ctr += sizeof(uint16_t);
				auto size = triCount * sizeof(Triangle);
				indBuf = (Triangle*)&data[ctr];
				ctr += size;
				break;
			}
			case '\x26':
			{
				uint16_t count = *(uint16_t*)&data[ctr];
				ctr += sizeof(uint16_t);
				mesh->Models.resize(count);
				for (auto& model : mesh->Models) {
					memcpy(&model.Material, &data[ctr], sizeof(SubMesh::Material));
					ctr += sizeof(SubMesh::Material);
					auto nameLength = data[ctr++];
					model.Name = CP1252_to_UTF8(&data[ctr], nameLength);
					ctr += nameLength;
				}
				break;
			}
			case '\x79':
			{
				// TODO need for transpose ??
				memcpy(&mesh->Origin, &data[ctr], sizeof(glm::mat4));
				ctr += sizeof(glm::mat4);
				break;
			}
			case '\x54':
			{
				// Read bone count
				uint16_t count = *(uint16_t*)&data[ctr];
				ctr += sizeof(uint16_t);

				if (!count) break; // Nothing to do here...
				mesh->Bones.resize(count);
				weights.resize(count);

				for (int i = 0; i < count; ++i) {
					auto& [weightPtr, weightCount] = weights[i];

					// Read bone name
					auto nameLength = data[ctr++];
					mesh->Bones[i] = CP1252_to_UTF8(&data[ctr], nameLength);
					ctr += nameLength;

					// Read bone weight count
					weightCount = *(uint16_t*)&data[ctr];
					ctr += sizeof(uint16_t);

					// Assign bone weights
					weightPtr = (Bone*)&data[ctr];
					ctr += weightCount * sizeof(Bone);
				}
				break;
			}
			default:
			{
				// TODO fail
				delete mesh;
				return nullptr;
			}
		}
	}

	// Assign weights
	if (weights.size()) {
		mesh->Weights.resize(
			mesh->Vertices.size(), 
			{ { -1ul, -1ul, -1ul, -1ul }, { 0.f, 0.f, 0.f, 0.f } });
		for (uint32_t bone = 0; bone < weights.size(); ++bone) {
			auto& [weightPtr, weightCount] = weights[bone];
			for (int v = 0; v < weightCount; ++v) {
				auto& [vertex, weight] = weightPtr[v];
				auto& [vIndices, vWeights] = mesh->Weights[vertex];
				if (glm::any(glm::equal(vIndices, glm::u32vec4(bone)))) {
#ifndef NDEBUG
					for (int i = 0; i < 4; ++i) {
						assert(vIndices[i] != bone || vWeights[i] == weight);
					}
#endif
					continue;
				}
				if (weight > vWeights.x) {
					vIndices.yzw = vIndices.xyz;
					vWeights.yzw = vWeights.xyz;
					vIndices.x = bone;
					vWeights.x = weight;
				}
				else if (weight > vWeights.y) {
					vIndices.zw = vIndices.yz;
					vWeights.zw = vWeights.yz;
					vIndices.y = bone;
					vWeights.y = weight;
				}
				else if (weight > vWeights.z) {
					vIndices.w = vIndices.z;
					vWeights.w = vWeights.z;
					vIndices.z = bone;
					vWeights.z = weight;
				}
				else if (weight > vWeights.w) {
					vIndices.w = bone;
					vWeights.w = weight;
				}
			}
		}
	}

	for (int mtl = 0; mtl < mesh->Models.size(); ++mtl) {
		auto& model = mesh->Models[mtl];
		for (int i = 0; i < triCount; ++i) {
			const auto& tri = indBuf[i];
			if (tri.material != mtl)
				continue;
			model.Indices.insert(model.Indices.end(), tri.indices, tri.indices + 3);
		}
	}

	return mesh;
}

void O3dLoader::FreeMesh(PMeshInstance instance) const
{
  auto mesh = static_cast<Mesh*>(instance);
  assert(mesh);
  delete mesh;
}

uint32_t O3dLoader::GetMaterialCount(PMeshInstance instance) const
{
  auto mesh = static_cast<Mesh*>(instance);
  assert(mesh);

  return (uint32_t)mesh->Models.size();
}

const char* O3dLoader::GetMaterialName(PMeshInstance instance, uint32_t materialId) const
{
  auto mesh = static_cast<Mesh*>(instance);
  assert(mesh);

  return mesh->Models[materialId].Name.c_str();
}

void O3dLoader::GetMaterialParams(
  simp::MeshLoaderApi::PMeshInstance instance,
  uint32_t materialId,
  float diffuse[],
  float ambient[],
  float specular[],
  float emit[],
  float& shininess,
  float& alpha) const
{
  auto mesh = static_cast<Mesh*>(instance);
  assert(mesh);

  const auto& matl = mesh->Models[materialId].Material;
  *(glm::vec3*)diffuse = matl.Diffuse;
  *(glm::vec3*)ambient = { 1.f, 1.f, 1.f };
  *(glm::vec3*)specular = matl.Specular;
  *(glm::vec3*)emit = matl.Emissive;
  shininess = matl.Shininess;
  alpha = matl.Alpha;
}

uint32_t O3dLoader::GetTriangleCount(PMeshInstance instance, uint32_t materialId) const
{
  auto mesh = static_cast<Mesh*>(instance);
  assert(mesh);

  return (uint32_t)(mesh->Models[materialId].Indices.size() / 3);
}

void O3dLoader::GetVertex(
  PMeshInstance instance,
  uint32_t materialId,
  uint32_t triangleId,
  uint32_t vertexId,
  float position[],
  float normal[],
  float texCoord[]) const
{
  auto mesh = static_cast<Mesh*>(instance);
  assert(mesh);

  const auto& vertex = mesh->Vertices[mesh->Models[materialId].Indices[triangleId * 3 + vertexId]];
  *(glm::vec3*)position = vertex.Position;
  *(glm::vec3*)normal = vertex.Normal;
  *(glm::vec2*)texCoord = vertex.TexCoord;
}

void O3dLoader::GetVertexWeights(
  PMeshInstance instance, 
  uint32_t materialId, 
  uint32_t triangleId,
  uint32_t vertexId,
  uint32_t bones[], 
  float weights[]) const
{
  auto mesh = static_cast<Mesh*>(instance);
  assert(mesh);

	const auto& vertexWeights = mesh->Weights[mesh->Models[materialId].Indices[triangleId * 3 + vertexId]];
	*(glm::u32vec4*)bones = vertexWeights.Indices;
	*(glm::vec4*)weights = vertexWeights.Weights;
}

uint32_t O3dLoader::GetBoneCount(PMeshInstance instance) const
{
  auto mesh = static_cast<Mesh*>(instance);
  assert(mesh);

  return mesh->Bones.size();
}

const char* O3dLoader::GetBoneName(PMeshInstance instance, uint32_t boneId) const
{
  auto mesh = static_cast<Mesh*>(instance);
  assert(mesh);

  return mesh->Bones[boneId].c_str();
}

void O3dLoader::GetOriginTransform(PMeshInstance instance, float matrix[]) const
{
  auto mesh = static_cast<Mesh*>(instance);
  assert(mesh);

  *(glm::mat4*)matrix = mesh->Origin;
}

