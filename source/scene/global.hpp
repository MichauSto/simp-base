#pragma once

#include "tile.hpp"

#include <filesystem>
#include <unordered_map>

namespace simp {

  struct Global {

    struct EntryPoint {
      int Tile;
      int Object;
      std::string Name;
      glm::mat4 Position;
    };

    Global(const std::filesystem::path& path);

    std::map<int, std::tuple<int, int>> ObjectLookup{};
    std::vector<EntryPoint> EntryPoints{};
    std::vector<MapTile> Tiles{};

    void Instantiate(Scene& scene, glm::ivec2 co, int radius) const;

  };

}