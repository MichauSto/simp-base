#pragma once

#include "visual/mesh.hpp"
#include "visual/texture.hpp"
#include "filesystem/cfgfile.hpp"
#include "utils/dict.hpp"

#include "scene/scene.hpp"

#include <memory>
#include <vector>
#include <filesystem>

namespace simp {

  struct MaterialBlueprint {
    struct Item {
      std::shared_ptr<Texture> DiffuseTexture;
    };
    Item Default{};
    std::vector<Item> Items{};
    int varIndex = -1;
  };

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

    void Instantiate(Scene& scene, entt::entity mapObject, entt::entity scriptObject) const;

    ModelBlueprint() = default;
    ModelBlueprint(
      const CfgFile& config,
      const std::filesystem::path& meshPath,
      const std::filesystem::path& texturePath,
      const Dict<int>& varLookup,
      const Dict<int>& stringVarLookup);

    std::vector<AnimBlueprint> Animations;
    std::vector<MeshBlueprint> Meshes;
    std::vector<LodBlueprint> Lods;

  };


}