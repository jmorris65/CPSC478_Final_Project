#include "POINT.h"

Point::Point (VEC3 coords)
  : Actor ()
{
  _center = coords;
}

bool Point::getRayIntersect (VEC3 e, VEC3 d, float &t, const Actor *&c, VEC3 &n) const
{
  t = _epsilon;
  c = nullptr;
  n = VEC3 (0.0, 1.0, 0.0);

  // Ray point intersection
  float t0 = (_center[0] - e[0]) / d[0],
	t1 = (_center[1] - e[1]) / d[1],
	t2 = (_center[2] - e[2]) / d[2];

  if (t0 != t1)
    return false;

  if (t1 != t2)
    return false;


  if (t0 > _epsilon) {
    t = t0;
    c = this;
    n = -1.0 * d;
    return true;
  }

  return false;
}

VEC3 Point::getNormal (VEC3 p) const
{
  // Return the normal
  VEC3 n = p / sqrt (p.dot (p));
  return -1.0 * n;
}

std::string Point::toString (void) const
{
  std::stringstream ss;
  ss	<< "Point" << _id
	<< ":\n\tCoords: " << _center[0]
	<< ", " << _center[1]
	<< ", " << _center[2];

  return ss.str ();
}

int Point::getRandomPoints (int n, std::vector<VEC3> &points) const
{
  points.push_back (_center);
  return 1;
}
