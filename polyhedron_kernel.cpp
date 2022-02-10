#include "polyhedron_kernel.h"

using namespace cinolib;

CINO_INLINE
void PolyhedronKernel::initialize(const std::vector<vec3d> &verts) {
  // initialize the kernel with the polyhedron AABB
  if (verts.empty())
    return;
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
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void PolyhedronKernel::compute(const std::vector<vec3d> &verts,
                               const std::vector<std::vector<uint>> &faces,
                               const std::vector<vec3d> &normals,
                               const bool &shuffle) {
  if (kernel_verts.empty() || kernel_faces.empty()) {
    std::cout << "ERROR: initialize kernel before computing." << std::endl;
    return;
  }
  std::vector<uint> faces_ids(faces.size());
  std::iota(faces_ids.begin(), faces_ids.end(), 0);
  if (shuffle) { // optional shuffle mode
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(faces_ids.begin(), faces_ids.end(), g);
  }

  for (uint fid : faces_ids) {
    std::vector<uint> f = faces.at(fid);
    std::vector<vec3d> v(f.size());
    for (uint vid = 0; vid < f.size(); vid++)
      v.at(vid) = verts.at(f.at(vid));
    if (f.size() == 0 || v.front().is_nan() || v.front().is_inf() ||
        normals.at(fid).is_deg()) {
      std::cout << "WARNING: skipping degenerate face." << std::endl;
      continue;
    }
    ExtendedPlane plane(v.front(), -normals.at(fid));

    std::vector<INTERSECTION_TYPE> v_sign(kernel_verts.size());
    for (uint vid = 0; vid < kernel_verts.size(); vid++) {
      if (find_v(v.begin(), v.end(), kernel_verts.at(vid)) != v.cend())
        v_sign.at(vid) = INTERSECT;
      else
        v_sign.at(vid) = contains(plane, kernel_verts.at(vid));
    }
    polyhedron_plane_intersection(kernel_verts, v_sign, kernel_faces, plane);
    if (kernel_verts.size() < 3 || kernel_faces.size() < 3) {
      kernel_verts.clear();
      kernel_faces.clear();
      return;
    }
  }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void PolyhedronKernel::polyhedron_plane_intersection(
    std::vector<vec3d> &verts, const std::vector<INTERSECTION_TYPE> &v_sign,
    std::vector<std::vector<uint>> &faces, const ExtendedPlane &plane) {
  std::vector<vec3d> above_v;
  std::vector<INTERSECTION_TYPE> above_s;
  std::vector<std::vector<uint>> above_f;

  for (uint fid = 0; fid < faces.size(); fid++) {
    std::vector<uint> f = faces.at(fid);
    std::vector<INTERSECTION_TYPE> fs(f.size());
    for (uint vid = 0; vid < f.size(); vid++)
      fs.at(vid) = v_sign.at(f.at(vid));
    switch (classify(fs)) {
    case BELOW: // face strictly below the plane
      break;
    case ABOVE: { // face weakly above the plane
      for (uint &vid : f) {
        auto it = find_v(above_v.begin(), above_v.end(), verts.at(vid));
        if (it == above_v.end()) {
          above_v.push_back(verts.at(vid));
          above_s.push_back(v_sign.at(vid));
          vid = above_v.size() - 1;
        } else
          vid = it - above_v.cbegin();
      }
      above_f.push_back(f);
      break;
    }
    case INTERSECT: { // face properly intersects the plane
      std::vector<vec3d> fv(f.size());
      for (uint vid = 0; vid < f.size(); vid++)
        fv.at(vid) = verts.at(f.at(vid));
      polygon_plane_intersection(fv, fs, f, plane);
      for (uint i = 0; i < f.size(); i++) {
        auto it = find_v(above_v.begin(), above_v.end(), fv.at(i));
        if (it == above_v.end()) {
          above_v.push_back(fv.at(i));
          above_s.push_back(fs.at(i));
          f.at(i) = above_v.size() - 1;
        } else
          f.at(i) = it - above_v.cbegin();
      }
      add_face(f, above_f);
      break;
    }
    default:
      break;
    }
  }
  verts = above_v;
  faces = above_f;

  std::vector<vec3d> cap_v; // generate the cap face
  std::vector<uint> cap_vids;
  for (uint vid = 0; vid < above_v.size(); vid++)
    if (above_s.at(vid) == INTERSECT) {
      cap_vids.push_back(vid);
      cap_v.push_back(above_v.at(vid));
    }
  if (cap_v.size() < 3)
    return;
  std::vector<uint> cap_f(cap_v.size());
  std::vector<uint> tmp_f = sort_points(cap_v, plane);
  for (uint i = 0; i < tmp_f.size(); i++)
    cap_f.at(i) = cap_vids.at(tmp_f.at(i));
  add_face(cap_f, faces);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void PolyhedronKernel::polygon_plane_intersection(
    std::vector<vec3d> &verts, std::vector<INTERSECTION_TYPE> &v_sign,
    std::vector<uint> &face, const ExtendedPlane &plane) {
  std::vector<vec3d> above_v;
  std::vector<INTERSECTION_TYPE> above_s;
  std::vector<uint> above_f;
  uint last = *std::max_element(face.cbegin(), face.cend());

  for (uint eid = 0; eid < face.size(); eid++) {
    uint vid1 = face.at((eid + 1) % face.size());
    vec3d v1 = verts.at((eid + 1) % verts.size());
    INTERSECTION_TYPE vs0 = v_sign.at(eid);
    INTERSECTION_TYPE vs1 = v_sign.at((eid + 1) % v_sign.size());
    switch (classify({vs0, vs1})) {
    case BELOW: { // edge weakly below the plane
      if (vs1 == INTERSECT) {
        above_v.push_back(v1);
        above_s.push_back(vs1);
        above_f.push_back(vid1);
      }
      break;
    }
    case ABOVE: { // edge weakly above the plane
      above_v.push_back(v1);
      above_s.push_back(vs1);
      above_f.push_back(vid1);
      break;
    }
    case INTERSECT: { // edge properly intersects the plane
      vec3d v = line_plane_intersection(verts.at(eid), v1, plane);
      last++;
      above_v.push_back(v);
      above_s.push_back(INTERSECT);
      above_f.push_back(last);
      if (vs0 == BELOW) {
        above_v.push_back(v1);
        above_s.push_back(vs1);
        above_f.push_back(vid1);
      }
      break;
    }
    default:
      break;
    }
  }
  verts = above_v;
  v_sign = above_s;
  face = above_f;
  assert(verts.size() == face.size());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d PolyhedronKernel::line_plane_intersection(const vec3d &v0,
                                                const vec3d &v1,
                                                const ExtendedPlane &plane) {
  vec3d p = v0.dist(plane.p) > TOLL ? plane.p : plane.points.at(1);
  double N = plane.n.dot(v0 - p);
  double D = plane.n.dot(v1 - v0);
  assert(fabs(D) > TOLL);
  return v0 - N / D * (v1 - v0);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
PolyhedronKernel::INTERSECTION_TYPE
PolyhedronKernel::classify(const std::vector<INTERSECTION_TYPE> &sign) {
  int size = sign.size();
  assert(size > 1);
  switch (size) {
  case 2: { // edge classification
    INTERSECTION_TYPE s0 = sign.front();
    INTERSECTION_TYPE s1 = sign.back();
    if (s0 <= 0 && s1 <= 0)
      return BELOW;
    else if (s0 >= 0 && s1 >= 0)
      return ABOVE;
    else
      return INTERSECT;
    break;
  }
  default: { // face classification
    int N = std::count(sign.cbegin(), sign.cend(), BELOW);
    if (N == size)
      return BELOW;
    else if (N == 0)
      return ABOVE;
    else
      return INTERSECT;
    break;
  }
  }
}
