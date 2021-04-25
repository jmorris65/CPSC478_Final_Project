#include "TRIANGLE.h"

// Initialization

// Implementations
Triangle::Triangle (VEC3 a, VEC3 b, VEC3 c)
  : _a {a},
    _b {b},
    _c {c},
    Actor ()
{
}

Triangle::Triangle (VEC3 a, VEC3 b, VEC3 c, VEC3 n)
  : Triangle (a,b,c)
{
  _normal = n;
}

void Triangle::getVertices (VEC3 &a, VEC3 &b, VEC3 &c) const
{
  a = _a;
  b = _b;
  c = _c;
}

bool Triangle::getRayIntersect (VEC3 _e, VEC3 _d, float &t, const Actor *&c, VEC3 &n) const
{
  t = _epsilon;
  c = nullptr;
  n = VEC3 (0.0, 1.0, 0.0);

  // Moller-Trumbore algorithm
  VEC3 vba = _b - _a;
  VEC3 vca = _c - _a;

  VEC3 vdca = _d.cross (vca);
  float d = vba.dot (vdca);
  d = 1 / d;

  VEC3 vea = _e - _a;
  float gamma = vea.dot (vdca) * d;
  if (gamma < 0 || gamma > 1)
    return false;

  VEC3 veba = vea.cross (vba);
  float beta = _d.dot (veba) * d;
  if (beta < 0 || beta > 1 - gamma)
    return false;

  float t0 = vca.dot (veba) * d;

  if (t0 > _epsilon) {
    t = t0;
    c = this;
    n = _normal;
    return true;
  }

  return false;
}

std::string Triangle::toString (void) const
{
  std::stringstream ss;
  ss	    << "Triangle" << _id
	    << ":\n\t a = " << _a[0] << ", " << _a[1] << ", " << _a[2]
	    << "\n\t b = " << _b[0] << ", " << _b[1] << ", " << _b[2]
	    << "\n\t c = " << _c[0] << ", " << _c[1] << ", " << _c[2];

  return ss.str ();
}

VEC3 Triangle::getNormal (VEC3 p) const
{
  return _normal;
}

void Triangle::setNormal (VEC3 n)
{
  _normal = n;
}

int Triangle::getRandomPoints (int n, std::vector<VEC3> &points) const
{
  float u, v;
  VEC3 v0 = _b - _a;
  VEC3 v1 = _c - _a;

  for (int i = 0; i < n; ++i) {
    u = jrand ();
    v = 1.0 - u;

    points.push_back (_a + u * v0 + v * v1);
  }
  return 0;
}

void Triangle::translate (VEC3 t) {
  
  MATRIX4 transl;

  transl << 1.0, 0.0, 0.0, t[0],
	    0.0, 1.0, 0.0, t[1],
	    0.0, 0.0, 1.0, t[2],
	    0.0, 0.0, 0.0, 1.0;

  _a = truncate (transl * extend (_a));
  _b = truncate (transl * extend (_b));
  _c = truncate (transl * extend (_c));
}

void Triangle::rotate (VEC3 r)
{
  MATRIX3 rx, ry, rz;

  float tx = r[0] * (M_PI / 180.0),
	ty = r[1] * (M_PI / 180.0),
	tz = r[2] * (M_PI / 180.0);

  rx << 1.0, 0.0, 0.0,
	0.0, cos (tx), -1.0 * sin (tx),
	0.0, sin (tx), cos(tx);

  ry << cos (ty), 0.0, sin (ty),
        0.0, 1.0, 0.0,
	-1.0 * sin (ty), 0.0, cos (ty);

  rz << cos (tz), -1.0 * sin (tz), 0.0,
        sin (tz), cos (tz), 0.0,
	0.0, 0.0, 1.0;

  _a = rz * ry * rx * _a;
  _b = rz * ry * rx * _b;
  _c = rz * ry * rx * _c;
  _normal = rz * ry * rx * _normal;
}
