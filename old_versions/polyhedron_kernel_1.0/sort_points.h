#ifndef SORT_POINTS_H
#define SORT_POINTS_H

#include <cinolib/cino_inline.h>
#include <cinolib/geometry/plane.h>
#include <cinolib/geometry/polygon_utils.h>
#include <cinolib/stl_container_utilities.h>

namespace cinolib {

CINO_INLINE
double angle(const vec2d &p1, const vec2d &p2) {
  double a = atan2(p2.y() - p1.y(), p2.x() - p1.x());
  return (a > 0) ? a : a + 2 * M_PI;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool compare(const std::pair<uint, vec2d> &p1,
             const std::pair<uint, vec2d> &p2) {
  vec2d c(0, 0);
  double a1 = angle(c, p1.second);
  double a2 = angle(c, p2.second);

  if (a1 != a2)
    return (a1 < a2);

  double d1 = c.dist(p1.second);
  double d2 = c.dist(p2.second);
  return (d1 < d2);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec2d polygon_centroid(const std::vector<vec2d> &verts) {
  vec2d c(0, 0);
  for (vec2d v : verts)
    c += v;
  c /= static_cast<double>(verts.size());
  return c;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// https://www.baeldung.com/cs/sort-points-clockwise
CINO_INLINE
std::vector<uint> sort_points(const std::vector<vec3d> &verts,
                              const Plane &plane) {
  std::vector<vec2d> points;
  assert(polygon_flatten(verts, points));

  vec2d c = polygon_centroid(points);
  for (vec2d &p : points)
    p -= c;

  std::vector<std::pair<uint, vec2d>> points_map;
  for (uint vid = 0; vid < points.size(); vid++)
    points_map.push_back(std::pair<uint, vec2d>(vid, points.at(vid)));
  std::sort(points_map.begin(), points_map.end(), compare);

  std::vector<uint> vids;
  for (auto &p : points_map)
    vids.push_back(p.first);

  // plane points towards the interior of the element
  // I want the points to be ordered counterclockwise from outside the element
  // the normal induced by the points has to be opposite wrt the plane normal
  vec3d u = verts.at(vids.at(1)) - verts.at(vids.at(0));
  vec3d v = verts.at(vids.at(2)) - verts.at(vids.at(0));
  vec3d w = u.cross(v);
  if (w.dot(plane.n) > 0)
    REVERSE_VEC(vids);
  return vids;
}

} // namespace cinolib

#endif // SORT_POINTS_H
