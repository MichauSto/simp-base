#pragma once

#include "blueprints/sceneryblueprint.hpp"
#include "blueprints/splineblueprint.hpp"
#include <map>

namespace simp {

  struct TerrainPlane {
    TerrainPlane(const std::filesystem::path& path);
    float GetHeight(const glm::vec3& pos) const;
    int Segments;
    std::vector<float> Heights;
  };

  struct MapTile {

    TerrainPlane TerrainPlane;

    MapTile(glm::ivec2 co, const std::filesystem::path& path, std::function<void(int, int)> scoLookupEntry);

    struct SceneryEntry {
      int Id;
      std::shared_ptr<SceneryBlueprint> Blueprint;
      glm::mat4 Position;
      std::vector<std::string> StringVars;
    };

    struct SplineEntry {
      int Id;
      std::shared_ptr<SplineBlueprint> Blueprint;
      glm::mat4 Position;
      SplineSegment Segment;
      mutable std::shared_ptr<Mesh> MeshCache;
    };

    void Instantiate(Scene& scene) const;
    glm::ivec2 Co;
    std::vector<SceneryEntry> Scenery{};
    std::vector<SplineEntry> Splines{};
  };


}