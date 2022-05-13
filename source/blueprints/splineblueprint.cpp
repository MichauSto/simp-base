#include "splineblueprint.hpp"
#include "filesystem/cfgfile.hpp"
#include "filesystem/file.hpp"
#include "simp.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <execution>

namespace simp {



  int SplineBlueprint::GetSegmentCount() const
  {
    int result = 0;
    for (const auto& p : Textures)
      result += p.GetSegmentCount();
    return result;
  }

  SplineBlueprint::SplineBlueprint(const std::filesystem::path& path)
  {
    auto texturePath = path.parent_path() / "Texture";
    CfgFile config(LoadFileText(Simp::GetOmsiPath() / path));
    SplineProfile* profileContext = nullptr;
    MaterialBlueprint::Item* matlContext = nullptr;
    for (config.Reset(); !config.AtEnd(); config.SkipEmptyLines()) {
      if (config.TestTag("[texture]")) {
        auto name = config.GetLine();
        auto& tex = Textures.emplace_back();
        matlContext = &tex.Material;
        matlContext->Textures[Material::Diffuse] = MaterialBlueprint::ParseTexture(name, texturePath);
      }
      if (matlContext && matlContext->ParseConfig(config, texturePath)) {
        continue;
      }
      else if (config.TestTag("[profile]")) {
        profileContext = &Textures[config.ReadInt()].Profiles.emplace_back();
      }
      else if (config.TestTag("[profilepnt]")) {
        if (!profileContext) {
          // No profile
          continue;
        }
        auto& point = profileContext->Points.emplace_back();
        config.ReadFloats((float*)&point.Position, 2);
        point.U = config.ReadFloat();
        point.V = config.ReadFloat();
      }
      else {
        config.NextLine();
      }
    }

    // Optimize away invalid geometry
    for (auto& texture : Textures) {
      texture.Profiles.erase(
        std::remove_if(
          texture.Profiles.begin(), 
          texture.Profiles.end(), 
          [](auto p) -> bool { return p.Points.size() < 2; }),
        texture.Profiles.end());
    }
    Textures.erase(
      std::remove_if(
        Textures.begin(),
        Textures.end(),
        [](auto t) -> bool { return t.Profiles.empty(); }),
      Textures.end());

    // Calculate normals
    for (auto& texture : Textures) {
      for (auto& profile : texture.Profiles) {
        for (int i = 0; i < profile.Points.size(); ++i) {
          if (i == 0) {
            // First point
            auto diff = glm::normalize(
              profile.Points[i + 1].Position - profile.Points[i].Position);
            profile.Points[i].Normal = { -diff.y, diff.x };
          }
          else if (i == profile.Points.size() - 1) {
            // Last point
            auto diff = glm::normalize(
              profile.Points[i].Position - profile.Points[i - 1].Position);
            profile.Points[i].Normal = { -diff.y, diff.x };
          }
          else {
            auto diff1 = glm::normalize(
              profile.Points[i + 1].Position - profile.Points[i].Position);
            auto diff2 = glm::normalize(
              profile.Points[i].Position - profile.Points[i - 1].Position);
            profile.Points[i].Normal = glm::vec2{ -diff1.y - diff2.y, diff1.x + diff2.x };
            if (!profile.Points[i].Normal.length()) {
              // zero-vector
              profile.Points[i].Normal = glm::normalize(diff2 - diff1);
            }
            else {
              profile.Points[i].Normal = glm::normalize(profile.Points[i].Normal);
            }
          }
        }
      }
    }
  }

  std::shared_ptr<Mesh> SplineBlueprint::CreateMesh(SplineSegment segment) const
  {
    if (Textures.empty()) return nullptr;
    segment.CalcDeltas();
    auto segmentCount = std::max(std::min(segment.GetSegmentCount(), GetMaxSegmentCount()), 1);

    std::vector<Vertex> vertices;
    std::vector<SubModel> models;

    vertices.resize(segmentCount * GetSegmentCount() * 6);
    for (const auto& texture : Textures) {
      int offset = 0;
      if (models.size()) offset = models.back().IndexOffset + models.back().IndexCount;
      auto& model = models.emplace_back();

      model.IndexOffset = offset;
      model.IndexCount = segmentCount * texture.GetSegmentCount() * 6;

      struct SplineQuadIterator {
        using difference_type = int;
        using value_type = std::pair<int, Vertex*>;
        using pointer = void;
        using reference = value_type&;
        using iterator_category = std::forward_iterator_tag;
        bool operator!= (const SplineQuadIterator& rhs){
          return segment != rhs.segment;
        }
        bool operator== (const SplineQuadIterator& rhs) {
          return segment == rhs.segment;
        }
        difference_type operator-(const SplineQuadIterator& rhs) {
          return segment - rhs.segment;
        }
        SplineQuadIterator& operator++() {
          segment += 1;
          vert += 6;
          return *this;
        }
        SplineQuadIterator operator++(int) {
          auto temp = *this;
          segment += 1;
          vert += 6;
          return temp;
        }
        std::pair<int, Vertex*> operator->() {
          return { segment, vert };
        }
        std::pair<int, Vertex*> operator*() {
          return { segment, vert };
        }
        int segment;
        Vertex* vert;
      };

      int profileIndexOffset = offset;
      for (const auto& profile : texture.Profiles) {
        for (int i = 1; i < profile.Points.size(); ++i) {
          const auto& lPoint = profile.Points[i - 1];
          const auto& rPoint = profile.Points[i];

          SplineQuadIterator begin{ 0, &vertices[profileIndexOffset] };
          SplineQuadIterator end{ segmentCount, nullptr };

          std::for_each(std::execution::par_unseq, begin, end, [&](std::pair<int, Vertex*> p) -> void {
              auto [i, v] = p;
              float lDist = (segment.Length * i) / segmentCount;
              float uDist = (segment.Length * (i + 1)) / segmentCount;
              Vertex verts[4];

              for (int i = 0; i < 4; ++i) {
                float _dist = i & 2 ? uDist : lDist;
                const auto& point = i & 1 ? rPoint : lPoint;

                // Get point position
                glm::vec4 pos = { point.Position.x, 0.f, point.Position.y, 1.f };

                // Apply skew
                _dist += segment.GetSkew(_dist) * pos.x;

                // Apply height and cant
                pos.z += segment.GetCant(_dist) * pos.x + segment.GetHeight(_dist);

                // Assign vertex data
                auto& vertex = verts[i];
                vertex.Position = segment.GetPointTransform(_dist) * pos;
                vertex.Normal = segment.GetPointTransform(_dist) * glm::vec4{ point.Normal.x, 0.f, point.Normal.y, 0.f };
                vertex.TexCoord.x = point.U;
                vertex.TexCoord.y = _dist * point.V;
              }

              v[0] = verts[0];
              v[1] = verts[2];
              v[2] = verts[1];
              v[3] = verts[2];
              v[4] = verts[3];
              v[5] = verts[1];
            });
        }
        profileIndexOffset += segmentCount * profile.GetSegmentCount() * 6;
      }
    }

    return std::make_shared<Mesh>(
      std::move(vertices), 
      std::move(models), 
      std::vector<std::string>{}, 
      std::vector<VertexWeights>{}, 
      glm::mat4{ 1.f });
  }

  int SplineBlueprint::GetMaxSegmentCount() const
  {
    auto c = GetSegmentCount();
    if (c <= 0) return 1;
    return 640 / c;
  }


  glm::mat4 SplineSegment::GetPointTransform(float dist)
  {
    if (Radius) {
      return glm::translate(glm::vec3(Radius, 0.f, 0.f)) *
        glm::eulerAngleZ(dist / Radius) *
        glm::translate(glm::vec3(-Radius, 0.f, 0.f));
    }
    else {
      return glm::translate(glm::vec3(0.f, dist, 0.f));
    }
  }

  float SplineSegment::GetHeight(float dist)
  {
    assert(DeltaH.has_value());
    if (dist > Length * .5f)
    {
      dist = Length - dist;
      return DeltaH.value() - (GradEnd * dist + _dG2 * dist * dist * .5f);
    }
    else
    {
      return GradStart * dist + _dG1 * dist * dist * .5f;
    }
  }

  float SplineSegment::GetCant(float dist)
  {
    return glm::mix(CantStart, CantEnd, dist / Length);
  }

  float SplineSegment::GetSkew(float dist)
  {
    return glm::mix(SkewStart, SkewEnd, dist / Length);
  }

  void SplineSegment::CalcDeltas()
  {
    if (!DeltaH.has_value()) {
      DeltaH = Length * (GradStart + (GradEnd - GradStart) * .5f);
    }
    float halfLength = Length * .5f;
    float gradMid = DeltaH.value() / halfLength - .5f * GradEnd - .5f * GradStart;
    _dG1 = (gradMid - GradStart) / halfLength;
    _dG1 = (gradMid - GradEnd) / halfLength;
  }

  int SplineSegment::GetSegmentCount()
  {
    // 1 segment per .5deg of curvature angle
    return glm::ceil(glm::abs(Length / Radius) / glm::radians(.5f));
    // TODO 1 segment per .25deg of gradient change
  }


  int SplineBlueprint::SplineProfile::GetSegmentCount() const
  {
    return std::max(0, (int)Points.size() - 1);
  }


  int SplineBlueprint::SplineTexture::GetSegmentCount() const
  {
    int result = 0;
    for (const auto& p : Profiles)
      result += p.GetSegmentCount();
    return result;
  }


}