#include "meshloader.hpp"
#include "simp/mesh/meshloader.hpp"
#include "visual/mesh.hpp"

using namespace simp;

namespace {
  typedef MeshLoaderApi::IMeshLoader* (__stdcall* InitMeshLoader)(const char*);
  typedef void(__stdcall* FreeMeshLoader)(MeshLoaderApi::IMeshLoader*);
}

MeshLoader::MeshLoader(const std::filesystem::path& omsiDir)
{
  for (const auto& entry : std::filesystem::recursive_directory_iterator(std::filesystem::current_path() / "plugins/mesh")) {
    if (entry.path().extension() != ".dll") continue;
    LoadImpl(entry.path().generic_string(), omsiDir.generic_string());
  }
}

void MeshLoader::LoadImpl(const std::string& path, const std::string& omsiDir)
{
  auto module = LoadLibrary(path.c_str());
  if (!module) {
    return;
  }
  auto iml = (InitMeshLoader)GetProcAddress(module, "InitMeshLoader");
  if (!iml) {
    FreeLibrary(module);
    return;
  }
  auto loader = iml(omsiDir.c_str());
  if (!loader) {
    FreeLibrary(module);
    return;
  }
  Loaders.emplace_back(module, loader);
}

MeshLoader::~MeshLoader()
{
  for (const auto& impl : Loaders) {
    auto fml = (FreeMeshLoader)GetProcAddress(impl.Module, "FreeMeshLoader");
    if (fml) {
      fml(impl.Loader);
    }
    else {

    }
    FreeModule(impl.Module);
  }
}

std::shared_ptr<Mesh> MeshLoader::LoadMesh(const std::string& data, const std::string& path) const
{
  for (const auto& [module, loader] : Loaders) {
    // Try to load mesh
    auto mesh = loader->LoadMesh(data.size(), data.data(), path.c_str());
    if (!mesh) continue;

    // Store model count
    auto modelCount = loader->GetMaterialCount(mesh);

    // Allocate temporary buffers
    std::vector<SubModel> models(modelCount);
    std::vector<Vertex> vertices{};
    std::vector<std::string> bones{};
    std::vector<VertexWeights> weights{};

    // Load origin matrix
    glm::mat4 origin{ 1.f };
    loader->GetOriginTransform(mesh, (float*)&origin);

    bones.resize(loader->GetBoneCount(mesh));
    for (uint32_t i = 0; i < bones.size(); ++i) {
      bones[i] = loader->GetBoneName(mesh, i);
    }

    // Load models
    for (uint32_t modelId = 0; modelId < modelCount; ++modelId) {
      auto vertexOffset = (uint32_t)vertices.size();
      auto& model = models[modelId];
      auto faceCount = loader->GetTriangleCount(mesh, modelId);
      model.IndexOffset = vertexOffset;
      model.IndexCount = faceCount * 3ull;
      model.Name = loader->GetMaterialName(mesh, modelId);
      loader->GetMaterialParams(
        mesh,
        modelId,
        (float*)&model.Diffuse,
        (float*)&model.Ambient,
        (float*)&model.Specular,
        (float*)&model.Emissive,
        model.Shininess,
        model.Alpha);
      vertices.resize(vertexOffset + model.IndexCount);
      if (bones.size()) weights.resize(vertexOffset + model.IndexCount);
      for (uint32_t face = 0; face < faceCount; ++face) {
        for (uint32_t vert = 0; vert < 3; ++vert) {
          auto& vertex = vertices[vertexOffset + face * 3ull + vert];
          loader->GetVertex(
            mesh, 
            modelId, 
            face, 
            vert, 
            (float*)&vertex.Position,
            (float*)&vertex.Normal, 
            (float*)&vertex.TexCoord);
          if (bones.size()) {
            auto& vertexWeights = weights[vertexOffset + face * 3ull + vert];
            loader->GetVertexWeights(
              mesh,
              modelId,
              face,
              vert,
              (uint32_t*)&vertexWeights.Indices,
              (float*)&vertexWeights.Weights);
          }
        }
      }
    }

    // Free loader resources
    loader->FreeMesh(mesh);

    return std::make_shared<Mesh>(
      std::move(vertices), 
      std::move(models), 
      std::move(bones), 
      std::move(weights), 
      origin);
  }

  return nullptr;
}
