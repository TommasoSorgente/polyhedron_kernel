#ifndef SORT_POINTS_H
#define SORT_POINTS_H

#include "extendedplane.h"
#include <cinolib/cino_inline.h>

namespace cinolib {

CINO_INLINE
bool compare(const std::pair<uint, vec2d> &p1,
             const std::pair<uint, vec2d> &p2) {
  vec2d c(0, 0);
  double a1 = atan2(p1.second.y() - c.y(), p1.second.x() - c.x());
  if (a1 <= 0)
    a1 += 2 * M_PI;
  double a2 = atan2(p2.second.y() - c.y(), p2.second.x() - c.x());
  if (a2 <= 0)
    a2 += 2 * M_PI;
  if (a1 != a2)
    return (a1 < a2);
  else
    return (c.dist(p1.second) < c.dist(p2.second));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// equivalent of cinolib's 'polygon_flatten' contained in
// geometry/polygon_utils.h using the ExtendedPlane class instead of Plane
CINO_INLINE
bool polygon_flatten_Ext(const std::vector<vec3d> &poly3d,
                         std::vector<vec2d> &poly2d) {
  poly2d.clear();
  poly2d.reserve(poly3d.size());

  ExtendedPlane best_fit(poly3d);
  if (best_fit.n.is_deg() || best_fit.n.norm() == 0)
    return false;

  vec3d Z = vec3d(0, 0, 1);
  vec3d axis = best_fit.n.cross(Z);
  double angle = best_fit.n.angle_rad(Z);
  mat3d R = mat3d::ROT_3D(axis, angle);

  for (auto p : poly3d) {
    vec3d tmp = best_fit.project_onto(p);
    tmp = R * tmp;
    poly2d.push_back(vec2d(tmp._vec)); // will drop z
  }
  return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// https://www.baeldung.com/cs/sort-points-clockwise
CINO_INLINE
std::vector<uint> sort_points(const std::vector<vec3d> &verts,
                              const ExtendedPlane &plane) {
  std::vector<vec2d> points;
  std::vector<uint> vids;
  if (!polygon_flatten_Ext(verts, points)) {
    std::cout << "WARNING: polygon_flatten(verts, points) failed!" << std::endl;
    return vids;
  }

  vec2d c(0, 0); // polygon centroid
  for (const vec2d &v : points)
    c += v;
  c /= static_cast<double>(points.size());
  for (vec2d &p : points)
    p -= c;

  std::vector<std::pair<uint, vec2d>> points_map;
  for (uint vid = 0; vid < points.size(); vid++)
    points_map.push_back(std::pair<uint, vec2d>(vid, points.at(vid)));
  std::sort(points_map.begin(), points_map.end(), compare);
  for (auto &p : points_map)
    vids.push_back(p.first);

  // plane points towards the interior of the element
  // I want the points to be ordered counterclockwise from outside the element
  // the normal induced by the points has to be opposite wrt the plane normal
  vec3d u = verts.at(vids.at(1)) - verts.at(vids.at(0));
  vec3d v = verts.at(vids.at(2)) - verts.at(vids.at(0));
  vec3d w = u.cross(v);
  if (w.dot(plane.n) > 0)
    std::reverse(vids.begin(), vids.end());
  return vids;
}

} // namespace cinolib

#endif // SORT_POINTS_H
