#include <extendedplane.h>

namespace cinolib {

CINO_INLINE
std::ostream &operator<<(std::ostream &in, const ExtendedPlane &plane) {
  in << "[Plane] " << plane.n.x() << " * x + " << plane.n.y() << " * y + "
     << plane.n.z() << " * z = " << plane.d << "\n";
  return in;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
ExtendedPlane::ExtendedPlane(const vec3d &p0, const vec3d &p1,
                             const vec3d &p2) {
  points.push_back(p0);
  points.push_back(p1);
  points.push_back(p2);
  vec3d u = p1 - p0;
  vec3d v = p2 - p0;
  vec3d norm = u.cross(v);

  if (p0.is_nan() || p0.is_inf() || norm.is_deg()) {
     std::cout << "WARNING : failed to set degenerate plane!" << std::endl;
    p = vec3d(0, 0, 0);
    n = vec3d(0, 0, 0);
    return;
  }
  p = p0;
  n = norm;
  n.normalize();
  d = n.dot(p0);
  assert(fabs(operator[](p0)) < 1e-10);
  assert(fabs(operator[](p1)) < 1e-10);
  assert(fabs(operator[](p2)) < 1e-10);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
ExtendedPlane::ExtendedPlane(const vec3d &point, const vec3d &normal) {
  set_plane(point, normal);
  assert(fabs(operator[](point)) < 1e-10);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// http://www.ilikebigbits.com/blog/2015/3/2/plane-from-points
CINO_INLINE
ExtendedPlane::ExtendedPlane(const std::vector<vec3d> &samples) {
  // centroid
  vec3d c(0, 0, 0);
  for (auto p : samples)
    c += p;
  c /= static_cast<double>(samples.size());
  // 3x3 covariance matrix
  double xx = 0.0;
  double yy = 0.0;
  double xy = 0.0;
  double yz = 0.0;
  double xz = 0.0;
  double zz = 0.0;
  for (auto p : samples) {
    vec3d pc = p - c;
    xx += pc.x() * pc.x();
    xy += pc.x() * pc.y();
    xz += pc.x() * pc.z();
    yy += pc.y() * pc.y();
    yz += pc.y() * pc.z();
    zz += pc.z() * pc.z();
  }
  double det_x = yy * zz - yz * yz;
  double det_y = xx * zz - xz * xz;
  double det_z = xx * yy - xy * xy;
  double det_max = std::max(det_x, std::max(det_y, det_z));

  vec3d n;
    if (det_max == det_x)
      n = vec3d(1.0, (xz * yz - xy * zz) / det_x, (xy * yz - xz * yy) / det_x);
    else if (det_max == det_y)
      n = vec3d((yz * xz - xy * zz) / det_y, 1.0, (xy * xz - yz * xx) / det_y);
    else if (det_max == det_z)
      n = vec3d((yz * xy - xz * yy) / det_z, (xz * xy - yz * xx) / det_z, 1.0);
    else
      assert(false);

  if (n.is_deg() || n.norm() == 0) {
    std::cerr << "WARNING : samples don't span a plane, using method #2"
              << std::endl;
    // http://www.ilikebigbits.com/2017_09_25_plane_from_points_2.html
    xx /= static_cast<double>(samples.size());
    xy /= static_cast<double>(samples.size());
    xz /= static_cast<double>(samples.size());
    yy /= static_cast<double>(samples.size());
    yz /= static_cast<double>(samples.size());
    zz /= static_cast<double>(samples.size());
    vec3d weighted_dir(0, 0, 0);
    {
      double det_x = yy * zz - yz * yz;
      vec3d axis_dir(det_x, xz * yz - xy * zz, xy * yz - xz * yy);
      double weight = det_x * det_x;
      if (weighted_dir.dot(axis_dir) < 0.0)
        weight = -weight;
      weighted_dir += axis_dir * weight;
    }
    {
      double det_y = xx * zz - xz * xz;
      vec3d axis_dir(xz * yz - xy * zz, det_y, xy * xz - yz * xx);
      double weight = det_y * det_y;
      if (weighted_dir.dot(axis_dir) < 0.0)
        weight = -weight;
      weighted_dir += axis_dir * weight;
    }
    {
      double det_z = xx * yy - xy * xy;
      vec3d axis_dir(xy * yz - xz * yy, xy * xz - yz * xx, det_z);
      double weight = det_z * det_z;
      if (weighted_dir.dot(axis_dir) < 0.0)
        weight = -weight;
      weighted_dir += axis_dir * weight;
    }
    n = weighted_dir;
  }
  set_plane(c, n);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void ExtendedPlane::set_plane(const vec3d &point, const vec3d &normal) {
  if (point.is_nan() || point.is_inf() || normal.is_deg()) {
     std::cout << "WARNING : failed to set degenerate plane!" << std::endl;
    p = vec3d(0, 0, 0);
    n = vec3d(0, 0, 0);
    return;
  }
  p = point;
  n = normal;
  n.normalize();
  d = n.dot(point);
  assert(fabs(operator[](point)) < 1e-10);

  // find three points on the plane (the first one is p)
  // https://math.stackexchange.com/questions/2563909/find-points-on-a-plane
  double A = n.x(), B = n.y(), C = n.z();
  double a = p.x(), b = p.y(), c = p.z();
  vec3d s, t;
  if (fabs(A) >= fabs(B) && fabs(A) >= fabs(C)) {
    double u = -B / A;
    double v = -C / A;
    s = vec3d(a + u, b + 1.0, c);
    t = vec3d(a + v, b, c + 1.0);
  } else if (fabs(B) >= fabs(A) && fabs(B) >= fabs(C)) {
    double u = -A / B;
    double v = -C / B;
    s = vec3d(a + 1.0, b + u, c);
    t = vec3d(a, b + v, c + 1.0);
  } else if (fabs(C) >= fabs(A) && fabs(C) >= fabs(B)) {
    double u = -A / C;
    double v = -B / C;
    s = vec3d(a + 1.0, b, c + u);
    t = vec3d(a, b + 1.0, c + v);
  }
  assert(fabs(operator[](s)) < 1e-10);
  assert(fabs(operator[](t)) < 1e-10);

  // order s and t according to the plane normal n
  points.push_back(p);
  vec3d N1 = (s - p).cross(t - p);
  vec3d N2 = (t - p).cross(s - p);
  if (N1.dot(n) < 0) {
    points.push_back(s);
    points.push_back(t);
  } else if (N2.dot(n) < 0) {
    points.push_back(t);
    points.push_back(s);
  } else
    std::cout << "ERROR : failed to set degenerate plane points!" << std::endl;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double ExtendedPlane::operator[](const vec3d &p) const {
  return (n.dot(p) - d);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool ExtendedPlane::operator=(const ExtendedPlane & P) const
{
    return (fabs(d-P.d) < 1e-10 && n.dist(P.n) < 1e-10);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// http://mathworld.wolfram.com/HessianNormalForm.html
// http://mathworld.wolfram.com/Point-PlaneDistance.html (eq. 13)
CINO_INLINE
double ExtendedPlane::point_plane_dist_signed(const vec3d &p) const {
  assert(fabs(n.norm() - 1.0) < 1e-10);
  vec3d u = p - this->p;
  return u.dot(n);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
double ExtendedPlane::point_plane_dist(const vec3d &p) const {
  return std::fabs(point_plane_dist_signed(p));
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
vec3d ExtendedPlane::project_onto(const vec3d &p) const {
  vec3d res = p - n * point_plane_dist_signed(p);
  // auto  err = point_plane_dist(res);
  // assert(err < 1e-7);
  return res;
}

} // namespace cinolib
