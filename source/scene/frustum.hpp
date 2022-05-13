#pragma once
#include <glm/glm.hpp>


namespace simp {

  struct Plane {
    glm::vec4 m_Components;
    glm::vec3 getOrigin() const;
    glm::vec3 getNormal() const;
    float getDistance() const;

    void normalize();
    Plane(const float& a_, const float& b_, const float& c_, const float& d_);
    Plane(const glm::vec4& components_);
    Plane(const glm::vec3& origin_, const glm::vec3& normal_);
    Plane() = default;
  };

  struct Frustum {
    Plane m_Planes[6];
    Frustum(const glm::mat4 viewProjection);
    bool testBb(const glm::vec3& min, const glm::vec3& max, const glm::mat4& world) const;
  };


}