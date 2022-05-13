#include "modelblueprint.hpp"
#include "simp.hpp"

#include "scene/animcomponents.hpp"
#include "scene/transformcomponents.hpp"
#include "scene/rendercomponents.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace simp {

  void ModelBlueprint::Instantiate(
    Scene& scene, 
    entt::entity mapObject,
    entt::entity scriptObject, 
    int renderType) const
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

      // Invalid controller binding -> node with no animation
      if (bp.varIndex < 0) continue;

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

    auto renderBox = scene.GetRegistry().create();

    // Local transform
    scene.GetRegistry().emplace<TransformComponent>(
      renderBox,
      glm::translate(.5f * (m_BoundsMax + m_BoundsMin)));

    // Transform parent
    scene.GetRegistry().emplace<TransformParentComponent>(
      renderBox,
      mapObject);

    scene.GetRegistry().emplace<RenderBoxComponent>(renderBox, m_BoundsMax - m_BoundsMin);

    uint32_t index = 0;
    std::vector<entt::entity> nodes{};
    for (const auto& mesh : Meshes) {
      if (mesh.Materials.empty()) 
        continue;
      auto parent = mesh.Animation >= 0 ? animNodes[mesh.Animation] : mapObject;
      nodes.resize(mesh.Materials.size());
      scene.GetRegistry().create(nodes.begin(), nodes.end());
      for (int i = 0; i < mesh.Materials.size(); ++i) {

        // TODO skip materials with invalid texture
        //if (!mesh.Materials[i].Default.T) continue;

        mesh.Materials[i].Instantiate(
          scene,
          nodes[i],
          scriptObject,
          [&](int index) { return nullptr; },
          [&](int index) { return nullptr; });

        // Local transform
        scene.GetRegistry().emplace<TransformComponent>(
          nodes[i],
          glm::mat4{ 1.f });

        // Transform parent
        scene.GetRegistry().emplace<TransformParentComponent>(
          nodes[i],
          parent);

        // Render order component
        scene.GetRegistry().emplace<RenderOrderComponent>(nodes[i], renderType, 0.f, index++);

        if(renderType >= 10)
          scene.GetRegistry().emplace<RenderDistanceComponent>(nodes[i], renderBox);

        // Render component
        scene.GetRegistry().emplace<RenderComponent>(
          nodes[i], 
          mesh.Mesh, 
          i);

        scene.GetRegistry().emplace<LodComponent>(
          nodes[i],
          renderBox,
          Lods[mesh.Lod].MinSize,
          Lods[mesh.Lod].MaxSize);

        if (mesh.VisibleVarIndex >= 0) {
          scene.GetRegistry().emplace<VisibleComponent>(
            nodes[i],
            scriptObject,
            mesh.VisibleVarIndex,
            mesh.VisibleCond);
        }

        if (mesh.Viewpoint) {
          scene.GetRegistry().emplace<ViewpointComponent>(
            nodes[i],
            mesh.Viewpoint);
        }
      }
    }
  }

  ModelBlueprint::ModelBlueprint(
    const CfgFile& config, 
    const std::filesystem::path& meshPath,
    const std::filesystem::path& texturePath,
    const std::function<int(std::string_view)>& varLookup,
    const std::function<int(std::string_view)>& stringVarLookup)
  {
    bool boundsDefined = false;
    m_BoundsMax = glm::vec3{ -std::numeric_limits<float>::max() };
    m_BoundsMin = glm::vec3{ std::numeric_limits<float>::max() };
    LodBlueprint* lodContext = nullptr;
    MeshBlueprint* meshContext = nullptr;
    AnimBlueprint* animContext = nullptr;
    MaterialBlueprint* materialContext = nullptr;
    MaterialBlueprint::Item* matlItemContext = nullptr;
    int* animParentId = nullptr;
    Dict<int> meshIdentLookup{};
    for (config.Reset(); !config.AtEnd(); config.SkipEmptyLines()) {
      if (config.TestTag("[VFDmaxmin]")) {
        config.ReadFloats((float*)&m_BoundsMin, 3);
        config.ReadFloats((float*)&m_BoundsMax, 3);
        boundsDefined = true;
      }
      else if (config.TestTag("[lod]")) {
        auto size = config.ReadFloat();
        float maxSize = std::numeric_limits<float>::max();
        if (Lods.size()) {
          maxSize = Lods.back().MinSize;
          if (maxSize <= size) {
            // Invalid LOD order
            continue;
          }
        }
        lodContext = &Lods.emplace_back();
        lodContext->MinSize = size;
        lodContext->MaxSize = maxSize;
        
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
            const auto& model = meshContext->Mesh->getModel(i);
            auto& item = meshContext->Materials[i].Default;
            // Load default material
            item.Textures[Material::Diffuse] =
              MaterialBlueprint::ParseTexture(model.Name, texturePath);
            item.Alpha = model.Alpha;
            item.Diffuse = model.Diffuse;
            item.Ambient = model.Ambient;
            item.Emissive = model.Emissive;
            item.Specular = model.Specular;
            item.Shininess = model.Shininess;
          }
          if (!boundsDefined) {
            m_BoundsMin = glm::min(m_BoundsMin, meshContext->Mesh->BoundsMin);
            m_BoundsMax = glm::max(m_BoundsMax, meshContext->Mesh->BoundsMax);
          }
        }
        animParentId = &meshContext->Animation;
      }
      else if (config.TestTag("[visible]")) {
        if (!meshContext) {
          // No mesh
          continue;
        }
        meshContext->VisibleVarIndex = varLookup(config.GetLine());
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
        materialContext->varIndex = varLookup(config.GetLine());
        matlItemContext = &materialContext->Default;
      }
      else if (config.TestTag("[matl_item]")) {
        if (!materialContext) {
          // No material
          continue;
        }
        matlItemContext = &materialContext->Items.emplace_back(materialContext->Default);
      }
      else if (matlItemContext && matlItemContext->ParseConfig(config, texturePath)) {
        continue;
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
        animContext->varIndex = varLookup(config.GetLine());
        animContext->Mode = AnimBlueprint::Rot;
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
        animContext->varIndex = varLookup(config.GetLine());
        animContext->Mode = AnimBlueprint::Trans;
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