#ifndef CINO_POLYHEDRON_KERNEL_H
#define CINO_POLYHEDRON_KERNEL_H

#include <cinolib/cino_inline.h>
#include <cinolib/geometry/plane.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/min_max_inf.h>
#include <sort_points.h>

namespace cinolib {

CINO_INLINE
bool polyhedron_kernel(const std::vector<vec3d> &verts,
                       const std::vector<std::vector<uint>> &faces,
                       const std::vector<vec3d> &normals,
                       std::vector<vec3d> &kernel_verts,
                       std::vector<std::vector<uint>> &kernel_faces);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void polyhedron_plane_intersection(std::vector<vec3d> &verts,
                                   std::vector<std::vector<uint>> &faces,
                                   const Plane &p);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void polygon_plane_intersection(std::vector<vec3d> &verts,
                                std::vector<uint> &face, const Plane &p);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d line_plane_intersection(const vec3d &v0, const vec3d &v1, const Plane &p);

} // namespace cinolib

#ifndef CINO_STATIC_LIB
#include "polyhedron_kernel.cpp"
#endif

#endif // CINO_POLYHEDRON_KERNEL_H
