#include "frustum.hpp"

namespace simp {

  Frustum::Frustum(const glm::mat4 viewProjection)
  {
    glm::mat4 m = glm::transpose(viewProjection);
    m_Planes[0] = { m[3] + m[0] };
    m_Planes[1] = { m[3] - m[0] };
    m_Planes[2] = { m[3] + m[1] };
    m_Planes[3] = { m[3] - m[1] };
    m_Planes[4] = { m[3] + m[2] };
    m_Planes[5] = { m[3] - m[2] };
  }

  bool Frustum::testBb(const glm::vec3& min, const glm::vec3& max, const glm::mat4& world) const
  {
    glm::vec4 p[8];
    for (int i = 0; i < 8; ++i) {
      p[i] = world * glm::vec4{
        (i & 1) ? max.x : min.x,
        (i & 2) ? max.y : min.y,
        (i & 4) ? max.z : min.z,
        1.f };
    }

    for (int i = 0; i < 6; ++i) {

      bool inside = false;

      for (int j = 0; j < 8; ++j) {
        if (dot(p[j], m_Planes[i].m_Components) > 0.f) {
          inside = true;
          break;
        }
      }

      if (!inside) return false;
    }

    return true;
  }

  glm::vec3 Plane::getOrigin() const
  {
    return m_Components.xyz * -m_Components.w;
  }

  glm::vec3 Plane::getNormal() const
  {
    return m_Components.xyz;
  }

  float Plane::getDistance() const
  {
    return -m_Components.w;
  }

  void Plane::normalize()
  {
    float w = glm::length((glm::vec3)m_Components.xyz);
    m_Components *= 1.f / w;
  }

  Plane::Plane(const float& a_, const float& b_, const float& c_, const float& d_)
    : m_Components(a_, b_, c_, d_)
  {
    normalize();
  }

  Plane::Plane(const glm::vec4& components_)
    : m_Components(components_)
  { 
    normalize();
  }

  Plane::Plane(const glm::vec3& origin_, const glm::vec3& normal_)
    : m_Components(normal_, -glm::dot(origin_, normal_))
  {
  }

}