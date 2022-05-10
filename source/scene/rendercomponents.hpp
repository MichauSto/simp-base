#pragma once

#include "visual/mesh.hpp"
#include "visual/texture.hpp"

#include <entt/entt.hpp>
#include <cstdint>
#include <memory>

namespace simp {

  struct VisibleComponent {
    entt::entity Controller;
    int VarIndex;
    float Condition;
    bool Visible;
  };

  struct RenderFlagComponent {
    bool Render = false;
  };

  struct ViewpointComponent {
    int Mask = 0;
  };

  struct LodComponent {
    entt::entity RefObject;
    float MinSize;
    float MaxSize;
  };

  struct RenderDistanceComponent {
    entt::entity RefObject;
  };

  struct RenderBoxComponent {
    RenderBoxComponent(const glm::vec3& _size);
    glm::vec3 Size;
    float Radius;
    float ScreenSize;
    float Distance;
  };

  struct RenderOrderComponent {
    RenderOrderComponent(uint8_t _renderType, float _distance, uint32_t _ordinal);
    union {
#pragma pack(push, 1)
      struct {
        uint16_t Mnr;
        uint8_t Mjr;
        uint32_t Distance;
        uint8_t RenderType;
      } Parameters;
#pragma pack(pop)
      uint64_t Order;
    };
    static uint32_t CalcDistanceUnsigned(const float& _dist);
  };

  struct RenderComponent {
    RenderComponent(
      const std::shared_ptr<Mesh>& _mesh, 
      int _index);
    std::shared_ptr<Mesh> m_Mesh;
    int m_Index;
    ComPtr<ID3D11Buffer> m_ModelBuffer;
  };

}