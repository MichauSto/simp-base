#include "modelblueprint.hpp"
#include "simp.hpp"

#include "scene/animcomponents.hpp"
#include "scene/transformcomponents.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace simp {

  void ModelBlueprint::Instantiate(
    Scene& scene, 
    entt::entity mapObject,
    entt::entity scriptObject) const
  {
    // Create entities for each animation node
    std::vector<entt::entity> animNodes(Animations.size());
    scene.GetRegistry().create(animNodes.begin(), animNodes.end());

    // Assign animation components
    for (int i = 0; i < animNodes.size(); ++i) {
      const auto& bp = Animations[i];

      // Local transform
      scene.GetRegistry().emplace<TransformComponent>(
        animNodes[i], 
        glm::mat4{ 1.f });

      // Transform parent
      scene.GetRegistry().emplace<TransformParentComponent>(
        animNodes[i], 
        bp.Parent >= 0 ? animNodes[bp.Parent] : mapObject);

      // Animation controller component
      scene.GetRegistry().emplace<AnimComponent>(
        animNodes[i],
        bp.OffsetMatrix,
        scriptObject,
        bp.varIndex,
        bp.Factor,
        bp.Offset);

      // Speed limit component (if set)
      if (bp.MaxSpeed)
        scene.GetRegistry().emplace<AnimSpeedComponent>(
          animNodes[i],
          bp.MaxSpeed);

      // Delay component (if set)
      if (bp.Delay)
        scene.GetRegistry().emplace<AnimDelayComponent>(
          animNodes[i],
          bp.Delay);

      // Animation type dummy
      switch (bp.Mode) {
      case AnimBlueprint::Trans:
        scene.GetRegistry().emplace<AnimTransComponent>(animNodes[i]);
        break;
      case AnimBlueprint::Rot:
        scene.GetRegistry().emplace<AnimRotComponent>(animNodes[i]);
        break;
      default:
        // The model creator messed up really bad
        // Some meaningful log message, I guess
        break;
      }
    }
  }

  ModelBlueprint::ModelBlueprint(
    const CfgFile& config, 
    const std::filesystem::path& meshPath,
    const std::filesystem::path& texturePath,
    const Dict<int>& varLookup,
    const Dict<int>& stringVarLookup)
  {
    LodBlueprint* lodContext = nullptr;
    MeshBlueprint* meshContext = nullptr;
    AnimBlueprint* animContext = nullptr;
    MaterialBlueprint* materialContext = nullptr;
    MaterialBlueprint::Item* matlItemContext = nullptr;
    int* animParentId = nullptr;
    Dict<int> meshIdentLookup{};
    for (config.Reset(); !config.AtEnd(); config.SkipEmptyLines()) {
      if (config.TestTag("[lod]")) {
        auto size = config.ReadFloat();
        if (Lods.size() && Lods.back().MinSize <= size) {
          // Invalid LOD order
          continue;
        }
        lodContext = &Lods.emplace_back();
        lodContext->MinSize = size;
      }
      else if (config.TestTag("[mesh]")) {
        if (Lods.empty()) {
          lodContext = &Lods.emplace_back();
          lodContext->MinSize = 0.f;
        }
        meshContext = &Meshes.emplace_back();
        materialContext = nullptr;
        matlItemContext = nullptr;
        animContext = nullptr;
        auto path = meshPath / config.GetLine();
        meshContext->Mesh = Simp::GetMeshManager().Get(path);
        meshContext->Animation = -1;
        meshContext->Lod = (int)(lodContext - Lods.data());
        if (meshContext->Mesh) {
          meshContext->Materials.resize(meshContext->Mesh->getModelCount());
          for (int i = 0; i < meshContext->Materials.size(); ++i) {
            // Load default material
            meshContext->Materials[i].Default.DiffuseTexture = 
              Simp::GetTextureManager().Get(texturePath / meshContext->Mesh->getModel(i).Name);
          }
        }
        animParentId = &meshContext->Animation;
      }
      else if (config.TestTag("[visible]")) {
        if (!meshContext) {
          // No mesh
          continue;
        }
        //auto name = config.GetLine();
        auto varEntry = varLookup.find(config.GetLine());
        meshContext->VisibleVarIndex = varEntry == varLookup.end() ? -1 : (*varEntry).second;
        meshContext->VisibleCond = config.ReadFloat();
      }
      else if (config.TestTag("[viewpoint]")) {
        if (!meshContext) {
          // No mesh
          continue;
        }
        meshContext->Viewpoint = config.ReadInt();
      }
      else if (config.TestTag("[matl]")) {
        if (!meshContext) {
          // No mesh
          continue;
        }
        if (!meshContext->Mesh) {
          // Mesh failed to load
          continue;
        }
        materialContext = nullptr;
        matlItemContext = nullptr;
        auto name = config.GetLine();
        auto rpi = config.ReadInt();
        auto index = meshContext->Mesh->getModelIndex(name, rpi);
        if (index < 0) {
          // Texture not found in mesh
          continue;
        }
        matlItemContext = &meshContext->Materials[index].Default;
      }
      else if (config.TestTag("[matl_change]")) {
        if (!meshContext) {
          // No mesh
          continue;
        }
        if (!meshContext->Mesh) {
          // Mesh failed to load
          continue;
        }
        materialContext = nullptr;
        matlItemContext = nullptr;
        auto name = config.GetLine();
        auto rpi = config.ReadInt();
        auto index = meshContext->Mesh->getModelIndex(name, rpi);
        if (index < 0) {
          // Texture not found in mesh
          continue;
        }
        materialContext = &meshContext->Materials[index];
        auto varEntry = varLookup.find(config.GetLine());
        materialContext->varIndex = varEntry == varLookup.end() ? -1 : (*varEntry).second;
        matlItemContext = &materialContext->Default;
      }
      else if (config.TestTag("[matl_item]")) {
        if (!materialContext) {
          // No material
          continue;
        }
        matlItemContext = &materialContext->Items.emplace_back(materialContext->Default);
      }
      else if (config.TestTag("[newanim]")) {
        if (!meshContext) {
          // No mesh
          continue;
        }
        assert(animParentId);
        int parent = std::exchange(*animParentId, (int)Animations.size());
        animContext = &Animations.emplace_back();
        animContext->Parent = parent;
        animParentId = &animContext->Parent;
      }
      else if (config.TestTag("origin_from_mesh")) {
        if (!animContext) {
          // No animation
          continue;
        }
        if (!meshContext->Mesh) {
          // Origin from mesh requested, but mesh failed to load
          continue;
        }
        animContext->OffsetMatrix = meshContext->Mesh->Origin;
      }
      else if (config.TestTag("origin_trans")) {
        if (!animContext) {
          // No animation
          continue;
        }
        glm::vec3 offset{};
        config.ReadFloats((float*)&offset, 3);
        animContext->OffsetMatrix = 
          glm::translate(offset) * 
          animContext->OffsetMatrix;
      }
      else if (config.TestTag("origin_rot_x")) {
        if (!animContext) {
          // No animation
          continue;
        }
        animContext->OffsetMatrix = 
          glm::eulerAngleX(config.ReadFloat()) * 
          animContext->OffsetMatrix;
      }
      else if (config.TestTag("origin_rot_y")) {
        if (!animContext) {
          // No animation
          continue;
        }
        animContext->OffsetMatrix =
          glm::eulerAngleY(config.ReadFloat()) *
          animContext->OffsetMatrix;
      }
      else if (config.TestTag("origin_rot_z")) {
        if (!animContext) {
          // No animation
          continue;
        }
        animContext->OffsetMatrix =
          glm::eulerAngleZ(config.ReadFloat()) *
          animContext->OffsetMatrix;
      }
      else if (config.TestTag("offset")) {
        if (!animContext) {
          // No animation
          continue;
        }
        animContext->Offset = config.ReadFloat();
      }
      else if (config.TestTag("delay")) {
        if (!animContext) {
          // No animation
          continue;
        }
        animContext->Delay = config.ReadFloat();
      }
      else if (config.TestTag("maxspeed")) {
        if (!animContext) {
          // No animation
          continue;
        }
        animContext->MaxSpeed = config.ReadFloat();
      }
      else if (config.TestTag("anim_rot")) {
        if (!animContext) {
          // No animation
          continue;
        }
        if (animContext->Mode != AnimBlueprint::None) {
          // Animation mode redefinition
          continue;
        }
        auto varEntry = varLookup.find(config.GetLine());
        animContext->Mode = AnimBlueprint::Rot;
        animContext->varIndex = varEntry == varLookup.end() ? -1 : (*varEntry).second;
        animContext->Factor = config.ReadFloat();
      }
      else if (config.TestTag("anim_trans")) {
        if (!animContext) {
          // No animation
          continue;
        }
        if (animContext->Mode != AnimBlueprint::None) {
          // Animation mode redefinition
          continue;
        }
        auto varEntry = varLookup.find(config.GetLine());
        animContext->Mode = AnimBlueprint::Trans;
        animContext->varIndex = varEntry == varLookup.end() ? -1 : (*varEntry).second;
        animContext->Factor = config.ReadFloat();
      }
      else if (config.TestTag("[mesh_ident]")) {
        if (!meshContext) {
          // No mesh
          continue;
        }
        auto key = config.GetLine();
        if (meshIdentLookup.contains(key)) {
          // Mesh ident redefinition
          continue;
        }
        meshIdentLookup[(std::string)key] = (int)(meshContext - Meshes.data());
      }
      else if (config.TestTag("[animparent]")) {
        if (!meshContext) {
          // No mesh
          continue;
        }
        assert(animParentId);
        auto p = meshIdentLookup.find(config.GetLine());
        if (p == meshIdentLookup.end()) {
          // Mesh ident not found
          continue;
        }
        *animParentId = Meshes[(*p).second].Animation;
      }
      else {
        config.NextLine();
      }
    }
  }

}