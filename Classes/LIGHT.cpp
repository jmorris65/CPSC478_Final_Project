#include "LIGHT.h"

// Initialization

// Implementations
Light::Light (std::shared_ptr<Actor> shape, VEC3 color, float intensity)
  : Actor (),
    _intensity {intensity}
{
  _color = color;
  _shape = shape;
}

int Light::getShadowRays (VEC3 p, int n, std::vector<VEC3> &v, std::vector<float> &j) const
{
  int i, count;
  std::vector<VEC3> points;
  points.reserve (v.capacity ());

  count = _shape->getRandomPoints (n, points);

  for (i = 0; i < count; i++) {
    VEC3 &u = v[i];
    float &k = j[i];

    u = points[i] - p;

    k = sqrt (u.dot (u));
    u = u / k;

  }

  return count;
}

float Light::getIntensity (void) const
{
  return _intensity;
}

bool Light::getRayIntersect (VEC3 e, VEC3 d, float &t, const Actor *&c, VEC3 &n) const
{
  return _shape->getRayIntersect (e, d, t, c, n);
}

std::string Light::toString (void) const
{
  std::stringstream ss;
  ss	    << "Light" << _id << ":\n";
  return ss.str () + _shape->toString ();
}

int Light::getRandomPoints (int n, std::vector<VEC3> &points) const
{
  return _shape->getRandomPoints (n, points);
}

VEC3 Light::getNormal (VEC3 p) const
{
  return _shape->getNormal (p);
}

bool Light::hasChild (unsigned long index) const
{
  return (index == _id) || _shape->hasChild (index);
}

VEC3 Light::getColor (void) const
{
  return _intensity * _color;
}
