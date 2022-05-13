#pragma once

#include "visual/mesh.hpp"
#include "materialblueprint.hpp"

#include <filesystem>
#include <optional>
#include <memory>

namespace simp {

  struct SplineSegment {
  public:
    float Length;
    float Radius;
    float GradStart;
    float GradEnd;
    std::optional<float> DeltaH;
    float CantStart;
    float CantEnd;
    float SkewStart;
    float SkewEnd;
    float VOffset;
    bool Mirror;
  private:
    friend struct SplineBlueprint;
    glm::mat4 GetPointTransform(float dist);
    float GetHeight(float dist);
    float GetCant(float dist);
    float GetSkew(float dist);
    void CalcDeltas();
    int GetSegmentCount();
    float _dG1;
    float _dG2;
  };

  struct SplineBlueprint {

    struct SplineProfilePnt {
      glm::vec2 Position;
      glm::vec2 Normal;
      float U;
      float V;
    };

    struct SplineProfile {
      std::vector<SplineProfilePnt> Points;
      int GetSegmentCount() const;
    };

    struct SplineTexture {
      std::vector<SplineProfile> Profiles;
      int GetSegmentCount() const;
      MaterialBlueprint::Item Material;
    };

    int GetSegmentCount() const;

    SplineBlueprint(const std::filesystem::path& path);

    std::shared_ptr<Mesh> CreateMesh(SplineSegment segment) const;

    int GetMaxSegmentCount() const;

    std::vector<SplineTexture> Textures;
  };

}