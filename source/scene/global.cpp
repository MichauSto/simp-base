#include "global.hpp"

#include "filesystem/file.hpp"
#include "simp.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace simp {



  Global::Global(const std::filesystem::path& path)
  {
    auto refPath = path.parent_path();

    auto config = CfgFile(LoadFileText(Simp::GetOmsiPath() / path));

    for (config.Reset(); !config.AtEnd(); config.SkipEmptyLines()) {
      if (config.TestTag("[entrypoints]")) {
        EntryPoints.resize(config.ReadInt());
        for (auto& entryPoint : EntryPoints) {
          entryPoint.Tile = config.ReadInt();
          entryPoint.Object = config.ReadInt();
          config.NextLine(); // TODO ??

          glm::vec3 Position;
          config.ReadFloats((float*)&Position, 3);

          glm::quat Orientation;
          Orientation.x = config.ReadFloat();
          Orientation.y = config.ReadFloat();
          Orientation.z = config.ReadFloat();
          Orientation.w = config.ReadFloat();

          const static glm::mat4 co{
              { 1.f, 0.f, 0.f, 0.f },
              { 0.f, 0.f, 1.f, 0.f },
              { 0.f, 1.f, 0.f, 0.f },
              { 0.f, 0.f, 0.f, 1.f } };

          entryPoint.Position = 
            co * 
            glm::translate(Position) * 
            glm::toMat4(Orientation) *
            co;

          config.NextLine(); // ??
          entryPoint.Name = config.GetLine();
        }
      }
      else if (config.TestTag("[map]")) {
        glm::ivec2 co;
        config.ReadInts((int*)&co, 2);
        auto name = config.GetLine();

        auto tid = Tiles.size();
        Tiles.emplace_back(
          co, 
          refPath / name, 
          [&](int id, int obj) { 
            ObjectLookup[id] = {tid, obj};
          });
      }
      else {
        config.NextLine();
      }
    }

  }

  void Global::Instantiate(Scene& scene, glm::ivec2 co, int radius) const
  {
    for (const auto& tile : Tiles) {
      auto cd = tile.Co - co;
      if (cd.x * cd.x + cd.y * cd.y > radius * radius) continue;
      tile.Instantiate(scene);
    }
  }


}