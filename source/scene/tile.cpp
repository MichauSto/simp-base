#include "tile.hpp"

#include "simp.hpp"
#include "scene/transformcomponents.hpp"
#include "scene/rendercomponents.hpp"
#include "filesystem/file.hpp"
#include "filesystem/cfgfile.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <execution>

namespace simp {

  MapTile::MapTile(
    glm::ivec2 co, 
    const std::filesystem::path& path, 
    std::function<void(int, int)> scoLookupEntry) 
    : Co(co), TerrainPlane(path.generic_string() + ".terrain")
  {

    auto config = CfgFile(LoadFileText(Simp::GetOmsiPath() / path));

    for (config.Reset(); !config.AtEnd(); config.SkipEmptyLines()) {
      if (config.TestTag("[object]")) {
        int pos = Scenery.size();
        auto& obj = Scenery.emplace_back();
        config.NextLine(); // Always zero??
        obj.Blueprint = Simp::GetSceneryBlueprintManager().Get(config.GetLine());
        obj.Id = config.ReadInt();
        scoLookupEntry(obj.Id, pos);

        // Read position (XYZ)
        glm::vec3 Position;
        Position.x = config.ReadFloat();
        Position.y = config.ReadFloat();
        Position.z = config.ReadFloat();

        if (obj.Blueprint && !obj.Blueprint->AbsHeight) {
          Position.z = TerrainPlane.GetHeight(Position);
        }

        // Read rotation (ZXY)
        glm::vec3 Rotation;
        config.ReadFloats((float*)&Rotation, 3);

        obj.Position =
          glm::translate(Position) *
          glm::eulerAngleZ(-glm::radians(Rotation.x));

        // Read stringvarlist
        obj.StringVars.resize(config.ReadInt());
        for (auto& s : obj.StringVars) {
          s = config.GetLine();
        }
      }
      else if (config.TestTag("[spline]")) {
        int pos = Splines.size();
        auto& obj = Splines.emplace_back();
        config.NextLine();
        obj.Blueprint = Simp::GetSplineBlueprintManager().Get(config.GetLine());
        obj.Id = config.ReadInt();
        config.NextLine();
        config.NextLine();
        // sliLookupEntry(obj.Id, pos);

        glm::vec3 Position;
        Position.x = config.ReadFloat();
        Position.z = config.ReadFloat();
        Position.y = config.ReadFloat();

        // Read heading
        float Hdg = config.ReadFloat();

        obj.Position =
          glm::translate(Position) *
          glm::eulerAngleZ(-glm::radians(Hdg));

        obj.Segment.Length = config.ReadFloat();
        obj.Segment.Radius = config.ReadFloat();
        obj.Segment.GradStart = config.ReadFloat() * .01f;
        obj.Segment.GradEnd = config.ReadFloat() * .01f;
        obj.Segment.DeltaH = {};
        obj.Segment.CantStart = config.ReadFloat() * .01f;
        obj.Segment.CantEnd = config.ReadFloat() * .01f;
        obj.Segment.SkewStart = config.ReadFloat();
        obj.Segment.SkewEnd = config.ReadFloat();
        obj.Segment.VOffset = config.ReadFloat();
        obj.Segment.Mirror = DictEqual(config.GetLine(), "mirror");
      }
      else if (config.TestTag("[spline_h]")) {
        int pos = Splines.size();
        auto& obj = Splines.emplace_back();
        config.NextLine();
        obj.Blueprint = Simp::GetSplineBlueprintManager().Get(config.GetLine());
        obj.Id = config.ReadInt();
        config.NextLine();
        config.NextLine();
        // sliLookupEntry(obj.Id, pos);

        glm::vec3 Position;
        Position.x = config.ReadFloat();
        Position.z = config.ReadFloat();
        Position.y = config.ReadFloat();

        // Read heading
        float Hdg = config.ReadFloat();

        obj.Position =
          glm::translate(Position) *
          glm::eulerAngleZ(-glm::radians(Hdg));

        obj.Segment.Length = config.ReadFloat();
        obj.Segment.Radius = config.ReadFloat();
        obj.Segment.GradStart = config.ReadFloat() * .01f;
        obj.Segment.GradEnd = config.ReadFloat() * .01f;
        obj.Segment.DeltaH = config.ReadFloat();
        obj.Segment.CantStart = config.ReadFloat() * .01f;
        obj.Segment.CantEnd = config.ReadFloat() * .01f;
        obj.Segment.SkewStart = config.ReadFloat();
        obj.Segment.SkewEnd = config.ReadFloat();
        obj.Segment.VOffset = config.ReadFloat();
        obj.Segment.Mirror = DictEqual(config.GetLine(), "mirror");
      }
      else {
        config.NextLine();
      }
    }

    std::for_each(std::execution::par_unseq, Splines.begin(), Splines.end(), [](SplineEntry& s) -> void {
      if (s.MeshCache || !s.Blueprint) return;
      s.MeshCache = s.Blueprint->CreateMesh(s.Segment);
      });
  }

  void MapTile::Instantiate(Scene& scene) const
  {
    for (const auto& spline : Splines) {
      if (!spline.Blueprint || !spline.MeshCache) continue;

      auto root = scene.GetRegistry().create();
      scene.GetRegistry().emplace<TransformWorldComponent>(root, Co);
      scene.GetRegistry().emplace<TransformComponent>(root, spline.Position);
      
      scene.GetRegistry().emplace<StaticComponent>(root, spline.MeshCache);

      auto renderBox = scene.GetRegistry().create();

      // Local transform
      scene.GetRegistry().emplace<TransformComponent>(
        renderBox,
        glm::translate(.5f * (spline.MeshCache->BoundsMin + spline.MeshCache->BoundsMax)));

      // Transform parent
      scene.GetRegistry().emplace<TransformParentComponent>(
        renderBox,
        root);

      scene.GetRegistry().emplace<RenderBoxComponent>(renderBox, spline.MeshCache->BoundsMax - spline.MeshCache->BoundsMin);
      std::vector<entt::entity> nodes(spline.Blueprint->Textures.size());
      scene.GetRegistry().create(nodes.begin(), nodes.end());
      for (int i = 0; i < nodes.size(); ++i) {
        scene.GetRegistry().emplace<LodComponent>(nodes[i], renderBox, 0.f, std::numeric_limits<float>::max());
        scene.GetRegistry().emplace<TransformParentComponent>(nodes[i], root);
        scene.GetRegistry().emplace<TransformComponent>(nodes[i], glm::mat4{ 1.f });
        scene.GetRegistry().emplace<RenderOrderComponent>(nodes[i], 4, 0.f, i);
        scene.GetRegistry().emplace<RenderComponent>(nodes[i], spline.MeshCache, i);

        auto& matl = scene.GetRegistry().emplace<MaterialComponent>(nodes[i]);
        matl.m_Material = spline.Blueprint->Textures[i].Material.GetMaterial(entt::null, nullptr, nullptr);
      }
    }
    for (const auto& scenery : Scenery) {
      scenery.Blueprint->Instantiate(scene, scenery.Position, Co);
    }
  }

  TerrainPlane::TerrainPlane(const std::filesystem::path& path)
  {
    auto value = LoadFileBinary(Simp::GetOmsiPath() / path);
    Segments = ((int*)value.data())[0];
    auto ptr = ((float*)value.data()) + 1;
    Heights = { ptr, ptr + (Segments + 1) * (Segments + 1) };
  }

  float TerrainPlane::GetHeight(const glm::vec3& pos) const
  {
    auto stride = Segments + 1;
    glm::vec2 p = pos * (Segments * (1.f / 300.f));
    glm::ivec2 ll = glm::floor(p);
    glm::ivec2 ur = ll + 1;
    p -= (glm::vec2)ll;
    ll = glm::clamp(ll, 0, Segments);
    ur = glm::clamp(ur, 0, Segments);
    float vll = Heights[ll.y * stride + ll.x];
    float vlr = Heights[ll.y * stride + ur.x];
    float vul = Heights[ur.y * stride + ll.x];
    float vur = Heights[ur.y * stride + ur.x];
    return glm::lerp(glm::lerp(vll, vlr, p.x), glm::lerp(vul, vur, p.x), p.y) + pos.z;
  }

}
