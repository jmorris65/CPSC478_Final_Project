#include "SPHERE.h"

Sphere::Sphere (VEC3 center, float radius)
  : _radius {radius},
    Actor ()
{
  _center = center;
}

bool Sphere::getRayIntersect (VEC3 e, VEC3 d, float &t, const Actor *&c, VEC3 &n) const
{
  t = _epsilon;
  c = nullptr;

  float A, B, C, radical, t0;
  VEC3 o = e - _center;

  A = d.dot (d);

  B = (2.0 * d).dot (o);

  C = o.dot (o) - (_radius * _radius);

  radical = (B * B) - (4.0 * A * C);

  if (radical < 0.0)
    return false;

  t0 = (-1.0 * B - sqrt (radical)) / (2.0 * A);

  if (t0 > _epsilon) {
    t = t0;
    c = this;
    n = ((e + t0 * d) - _center).normalized ();
    return true;
  }

  t0 = (-1.0 * B + sqrt (radical)) / (2.0 * A);

  if (t0 > _epsilon) {
    t = t0;
    c = this;
    n = ((e + t0 * d) - _center).normalized ();
    return true;
  }

  return false;
}

std::string Sphere::toString (void) const
{
  std::stringstream ss;
  ss        << "Sphere" << _id
	    << ":\n\tCenter: " << _center[0]
	    << ", " << _center[1]
	    << ", " << _center[2]
	    << "\n\tRadius: " << _radius;

  return ss.str();
}

VEC3 Sphere::getNormal (VEC3 p) const
{
  VEC3 n = p - _center;
  n = n / sqrt(n.dot (n));
  return n;
}

int Sphere::getRandomPoints (int n, std::vector<VEC3> &points) const
{
  VEC3 p;

  for (int i = 0; i < n; ++n) {
    p[0] = jrand () * 2.0 - 1.0;
    p[1] = jrand () * 2.0 - 1.0;
    p[2] = jrand () * 2.0 - 1.0;

    p = p / sqrt (p.dot (p));

    p = (p * _radius) + _center;

    points.push_back (p);
  }

  return n;
}
