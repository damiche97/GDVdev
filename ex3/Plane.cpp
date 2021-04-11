#include "Plane.h"

Plane::Plane(
  std::Vec3f normal,
  float dist0
)
  : normal(std::move(normal)),
    dist0(dist0)
{
  const auto normalization_factor = this->normal.length();
  this->normal.normalize();
  // We also need to normalize the distance from the origin by the same factor,
  // or we will get slightly wrong results
  // http://www.crownandcutlass.com/features/technicaldetails/frustum.html
  this->dist0 /= normalization_factor;
}

bool Plane::isInNormalDirection(std::Vec3f const & vec) const {
  return normal * vec + dist0 >= 0;
}

Plane Plane::fromTwoPoints(std::Vec3f const & a, std::Vec3f const & b) {
  //const std::Vec3f normal = (b-a)^(c-a);
  const std::Vec3f normal = b-a;

  return {
    normal,
    normal.x * a.x + normal.y * a.y + normal.z * a.z
  };
}

std::ostream & operator<<(std::ostream & lhs, Plane const & rhs) {
  return lhs << "Normal " << rhs.normal << " Dist " << rhs.dist0;
}
