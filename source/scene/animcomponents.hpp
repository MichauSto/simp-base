#pragma once

#include "entt/entt.hpp"

#include <glm/glm.hpp>

namespace simp {

  struct AnimComponent {
    AnimComponent(
      const glm::mat4& _worldMatrix,
      entt::entity _scriptObject,
      int _varIndex,
      float _factor,
      float _offset);
    glm::mat4 WorldMatrix;
    glm::mat4 InverseWorldMatrix;
    // TODO consider replacing with raw pointer
    entt::entity Controller;
    int VarIndex;

    float Factor;
    float Offset;

    float Var;
    float PrevVar;
    float CachedVar;
  };

  struct AnimSpeedComponent {
    float MaxSpeed;
  };

  struct AnimDelayComponent {
    float Delay;
  };

  struct AnimRotComponent {
    int Dummy;
  };

  struct AnimTransComponent {
    int Dummy;
  };

  struct AnimDirtyComponent {
    int Dummy;
  };


}