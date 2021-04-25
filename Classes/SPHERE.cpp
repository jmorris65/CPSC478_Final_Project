#include "SPHERE.h"

Sphere::Sphere (VEC3 center, float radius)
  : _radius {radius},
    Actor ()
{
  _center = center;
}

bool Sphere::getRayIntersect (VEC3 _e, VEC3 _d, std::pair<float, const Actor *> &v) const
{
  v.first = _epsilon;
  v.second = nullptr;

  float A, B, C, radical, t;
  VEC3 o = _e - _center;

  A = _d.dot (_d);

  B = (2.0 * _d).dot (o);

  C = o.dot (o) - (_radius * _radius);

  radical = (B * B) - (4.0 * A * C);

  if (radical < 0.0)
    return false;

  t = (-1.0 * B - sqrt (radical)) / (2.0 * A);

  if (t > _epsilon) {
    v.first = t;
    v.second = this;
    return true;
  }

  t = (-1.0 * B + sqrt (radical)) / (2.0 * A);

  if (t > _epsilon) {
    v.first = t;
    v.second = this;
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
