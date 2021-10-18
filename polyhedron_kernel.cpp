#include "polyhedron_kernel.h"

namespace cinolib {

CINO_INLINE
double contains(const Plane &P, const vec3d &p) {
  double d = P.operator[](p);
  return ((fabs(d) < 1e-10) ? 0.0 : d);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
std::vector<vec3d>::iterator find_v(std::vector<vec3d>::iterator first,
                                    std::vector<vec3d>::iterator last,
                                    const vec3d &v) {
  for (; first != last; ++first) {
    vec3d w = *first;
    if (w.dist(v) < 1e-8)
      break;
  }
  return first;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool polyhedron_kernel(const std::vector<vec3d> &verts,
                       const std::vector<std::vector<uint>> &faces,
                       const std::vector<vec3d> &normals,
                       std::vector<vec3d> &kernel_verts,
                       std::vector<std::vector<uint>> &kernel_faces) {
  if (verts.empty() || faces.empty())
    return false;
  // It could be useful to check here if the polyhedron is convex or not, before
  // starting with the kernel computation.

  // compute AABB
  vec3d min(inf_double, inf_double, inf_double);
  vec3d max(-inf_double, -inf_double, -inf_double);
  for (const vec3d &p : verts) {
    min = min.min(p);
    max = max.max(p);
  }
  kernel_verts = {min,
                  vec3d(min.x(), max.y(), min.z()),
                  vec3d(max.x(), max.y(), min.z()),
                  vec3d(max.x(), min.y(), min.z()),
                  vec3d(min.x(), min.y(), max.z()),
                  vec3d(min.x(), max.y(), max.z()),
                  max,
                  vec3d(max.x(), min.y(), max.z())};
  kernel_faces = {{0, 1, 2, 3}, {2, 1, 5, 6}, {3, 2, 6, 7},
                  {0, 3, 7, 4}, {1, 0, 4, 5}, {5, 4, 7, 6}};

  // compute kernel
  for (uint fid = 0; fid < faces.size(); fid++) {
    std::vector<uint> f = faces.at(fid);
    vec3d p = verts.at(f.front());
    if (p.norm() == 0)
      p = verts.at(f.at(1));
    vec3d n = normals.at(fid);
    Plane plane(p, -n);
    polyhedron_plane_intersection(kernel_verts, kernel_faces, plane);
  }
  return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void polyhedron_plane_intersection(std::vector<vec3d> &verts,
                                   std::vector<std::vector<uint>> &faces,
                                   const Plane &plane) {
  std::vector<vec3d> above_v;
  std::vector<std::vector<uint>> above_f;
  std::vector<double> v_sign;
  for (const vec3d &v : verts)
    v_sign.push_back(contains(plane, v));

  for (const std::vector<uint> &f : faces) {
    uint pos = 0, neg = 0, null = 0; // evaluate the sign of the face
    for (const uint &v : f) {
      if (v_sign.at(v) == 0.0)
        ++null;
      else if (v_sign.at(v) > 0)
        ++pos;
      else
        ++neg;
    }

    if (neg == f.size()) // face strictly below the plane
      continue;
    else if (pos + null == f.size()) { // face weakly above the plane
      std::vector<uint> face = f;
      for (uint &vid : face) {
        vec3d v = verts.at(vid);
        auto it = find_v(above_v.begin(), above_v.end(), v);
        if (it == above_v.end()) {
          above_v.push_back(v);
          vid = above_v.size() - 1;
        } else
          vid = it - above_v.begin();
      }
      above_f.push_back(face);
    } else { // face intersects the plane
      std::vector<uint> new_f = f;
      std::vector<vec3d> new_v;
      for (const uint &vid : f)
        new_v.push_back(verts.at(vid));
      polygon_plane_intersection(new_v, new_f, plane);
      for (uint i = 0; i < new_f.size(); i++) {
        vec3d v = new_v.at(i);
        auto it = find_v(above_v.begin(), above_v.end(), v);
        if (it == above_v.end()) {
          above_v.push_back(v);
          new_f.at(i) = above_v.size() - 1;
        } else
          new_f.at(i) = it - above_v.begin();
      }
      if (new_f.size() > 2)
        above_f.push_back(new_f);
    }
  }
  verts = above_v;
  faces = above_f;

  std::vector<vec3d> cap_v; // generate the cap face
  std::vector<uint> cap_f;
  for (const vec3d &v : above_v)
    if (contains(plane, v) == 0)
      cap_v.push_back(v);
  if (cap_v.size() < 3)
    return;
  std::vector<uint> tmp_f = sort_points(cap_v, plane);
  for (const uint &vid : tmp_f) {
    vec3d v = cap_v.at(vid);
    auto it = find_v(above_v.begin(), above_v.end(), v);
    cap_f.push_back(it - above_v.begin());
  }
  tmp_f = cap_f;
  SORT_VEC(tmp_f);
  for (std::vector<uint> &f : above_f) {
    SORT_VEC(f);
    if (tmp_f == f) // cap face already exists
      return;
  }
  faces.push_back(cap_f);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void polygon_plane_intersection(std::vector<vec3d> &verts,
                                std::vector<uint> &face, const Plane &plane) {
  std::vector<vec3d> above_v;
  std::vector<uint> above_f;
  uint last = *std::max_element(face.begin(), face.end());
  std::vector<double> v_sign;
  for (const vec3d &v : verts)
    v_sign.push_back(contains(plane, v));

  for (uint i = 0; i < face.size(); i++) {
    uint vid1 = face.at((i + 1) % face.size());
    vec3d v0 = verts.at(i);
    vec3d v1 = verts.at((i + 1) % verts.size());
    double d0 = v_sign.at(i);
    double d1 = v_sign.at((i + 1) % verts.size());

    if (d0 <= 0 && d1 < 0) // edge below the plane
      continue;
    else if (d0 <= 0 && d1 == 0) { // edge below, touching the plane in d1
      above_v.push_back(v1);
      above_f.push_back(vid1);
    } else if (d0 >= 0 && d1 >= 0) { // edge above the plane
      above_v.push_back(v1);
      above_f.push_back(vid1);
    } else if (d0 > 0) { // edge intersects the plane
      vec3d v = line_plane_intersection(v0, v1, plane);
      last++;
      above_v.push_back(v);
      above_f.push_back(last);
    } else if (d0 < 0) { // edge intersects the plane
      vec3d v = line_plane_intersection(v0, v1, plane);
      last++;
      above_v.push_back(v);
      above_f.push_back(last);
      above_v.push_back(v1);
      above_f.push_back(vid1);
    }
  }
  verts = above_v;
  face = above_f;
  assert(verts.size() == face.size());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d line_plane_intersection(const vec3d &v0, const vec3d &v1,
                              const Plane &plane) {
  double N = plane.n.dot(v0 - plane.p);
  double D = plane.n.dot(v1 - v0);
  N = (fabs(N) < 1e-10) ? 0.0 : N; // avoid numerical errors
  D = (fabs(D) < 1e-10) ? 0.0 : D;
  assert(D != 0);
  return v0 - N / D * (v1 - v0);
}

} // namespace cinolib
