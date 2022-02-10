#ifndef POLYHEDRON_KERNEL_H
#define POLYHEDRON_KERNEL_H

#include "extendedplane.h"
#include "sort_points.h"
#include <cinolib/cino_inline.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/min_max_inf.h>
#include <cinolib/predicates.h>

using namespace cinolib;

class PolyhedronKernel {

public:
  std::vector<vec3d> kernel_verts;
  std::vector<std::vector<uint>> kernel_faces;

  CINO_INLINE
  explicit PolyhedronKernel() {}

  CINO_INLINE
  void initialize(const std::vector<vec3d> &verts);

  CINO_INLINE
  void compute(const std::vector<vec3d> &verts,
               const std::vector<std::vector<uint>> &faces,
               const std::vector<vec3d> &normals,
               const bool &random_faces = false);

private:
  double TOLL = 1e-8;

  enum INTERSECTION_TYPE {
    BELOW = -1,
    INTERSECT = 0,
    ABOVE = 1,
  };

  CINO_INLINE
  void polyhedron_plane_intersection(
      std::vector<vec3d> &verts, const std::vector<INTERSECTION_TYPE> &v_sign,
      std::vector<std::vector<uint>> &faces, const ExtendedPlane &p);

  CINO_INLINE
  void polygon_plane_intersection(std::vector<vec3d> &verts,
                                  std::vector<INTERSECTION_TYPE> &v_sign,
                                  std::vector<uint> &face,
                                  const ExtendedPlane &p);

  CINO_INLINE
  vec3d line_plane_intersection(const vec3d &v0, const vec3d &v1,
                                const ExtendedPlane &p);

  CINO_INLINE
  INTERSECTION_TYPE classify(const std::vector<INTERSECTION_TYPE> &sign);

  CINO_INLINE
  void add_face(const std::vector<uint> &new_f,
                std::vector<std::vector<uint>> &faces) {
    if (new_f.size() < 3)
      return;
    std::vector<uint> tmp = SORT_VEC(new_f);
    for (const std::vector<uint> &f : faces)
      if (tmp == SORT_VEC(f))
        return; // new_f is already in faces
    faces.push_back(new_f);
  }

  CINO_INLINE
  INTERSECTION_TYPE contains(const ExtendedPlane &P, const vec3d &p) {
    double d = orient3d(P.points.at(0), P.points.at(1), P.points.at(2), p);
    if (fabs(d) < TOLL)
      return INTERSECT;
    else if (d > 0)
      return ABOVE;
    else
      return BELOW;
  }

  CINO_INLINE
  std::vector<vec3d>::iterator find_v(std::vector<vec3d>::iterator first,
                                      std::vector<vec3d>::iterator last,
                                      const vec3d &v) {
    for (; first != last; ++first)
      if (fabs((*first).x() - v.x()) < TOLL)
        if (fabs((*first).y() - v.y()) < TOLL)
          if (fabs((*first).z() - v.z()) < TOLL)
            break;
    return first;
  }
};

#ifndef CINO_STATIC_LIB
#include "polyhedron_kernel.cpp"
#endif

#endif // POLYHEDRON_KERNEL_H
