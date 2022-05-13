#pragma once

#include "visual/mesh.hpp"
#include "filesystem/cfgfile.hpp"
#include "utils/dict.hpp"
#include "materialblueprint.hpp"

#include "scene/scene.hpp"

#include <memory>
#include <vector>
#include <filesystem>

namespace simp {

  struct AnimBlueprint {
    enum struct AnimMode {
      None,
      Trans,
      Rot
    } Mode = None;
    using enum AnimMode;
    int Parent = -1;
    glm::mat4 OffsetMatrix{ 1.f };
    float Offset = 0.f;
    float Delay = 0.f;
    float MaxSpeed = 0.f;
    float Factor = 0.f;
    int varIndex = -1;
  };

  struct LodBlueprint {
    float MinSize = 0.f;
    float MaxSize = std::numeric_limits<float>::max();
  };

  struct MeshBlueprint {
    std::shared_ptr<Mesh> Mesh = nullptr;
    int Lod = -1;
    int Viewpoint = 0;
    int Animation = -1;
    std::vector<MaterialBlueprint> Materials{};
    int VisibleVarIndex = -1;
    float VisibleCond = 0.f;
  };

  struct ModelBlueprint {

    void Instantiate(Scene& scene, entt::entity mapObject, entt::entity scriptObject, int renderType) const;

    ModelBlueprint() = default;
    ModelBlueprint(
      const CfgFile& config,
      const std::filesystem::path& meshPath,
      const std::filesystem::path& texturePath,
      const std::function<int(std::string_view)>& varLookup,
      const std::function<int(std::string_view)>& stringVarLookup);

    glm::vec3 m_BoundsMax;
    glm::vec3 m_BoundsMin;

    std::vector<AnimBlueprint> Animations;
    std::vector<MeshBlueprint> Meshes;
    std::vector<LodBlueprint> Lods;

  };


}