#ifndef EXTENDEDPLANE_H
#define EXTENDEDPLANE_H

// extended version of cinolib::plane, with the additional information of three
// points contained in the plane, useful for shewchuck predicates

#include <cinolib/geometry/vec_mat.h>
#include <iostream>
#include <vector>

namespace cinolib {

// represents a plane with equation
// a*x + b*y + c*z = d
// or, using the Hessian Normal Form
// (https://en.wikipedia.org/wiki/Hesse_normal_form) n dot (x,y,z) - d = 0

class ExtendedPlane {

public:
  vec3d n;  // plane normal (i.e. a,b,c, coefficients of the plane equation)
  double d; // d coefficient of the plane equation
  vec3d p;  // any point on the plane (useful for point_plane_dist)
  std::vector<vec3d> points; // three points on the plane (the first one is p)

  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  explicit ExtendedPlane(const vec3d &point = vec3d(0, 0, 0),
                         const vec3d &normal = vec3d(0, 0, 1));

  explicit ExtendedPlane(const vec3d &p0, const vec3d &p1, const vec3d &p2);

  explicit ExtendedPlane(
      const std::vector<vec3d> &samples); // best fitting plane

  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  double a() const { return n.x(); }
  double b() const { return n.y(); }
  double c() const { return n.z(); }

  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  void set_plane(const vec3d &point, const vec3d &normal);

  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  double operator[](const vec3d &p) const;

  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  bool operator=(const ExtendedPlane & P) const;

  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  double point_plane_dist_signed(const vec3d &p) const;
  double point_plane_dist(const vec3d &p) const;
  vec3d project_onto(const vec3d &p) const;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::ostream &operator<<(std::ostream &in, const ExtendedPlane &plane);

} // namespace cinolib

#ifndef CINO_STATIC_LIB
#include "extendedplane.cpp"
#endif

#endif // EXTENDEDPLANE_H
