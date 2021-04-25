#include "ACTOR.h"

VEC3 truncate (const VEC4 &v)
{
  return VEC3 (v[0], v[1], v[2]);
}

VEC4 extend (const VEC3 &v)
{
  return VEC4 (v[0], v[1], v[2], 1.0);
}

float clamp (float t)
{
  if (t < 0.0)
    t = 0.0;
  else if (t > 1.0)
    t = 1.0;
  return t;
}

// Actor implementations
unsigned long Actor::_allocated = 0;
std::vector<float> Actor::J_CONSTANTS {1.0, 1.0, 1.5};

Actor::Actor (void)
{
  _center = VEC3 (0.0, 0.0, 0.0);
  _color = VEC3 (0.0, 0.0, 0.0);
  _reflective = false;
  _refractive = false;
  _material = J_NONE;
  _id = _allocated;
  _allocated = _allocated + 1;
}

VEC3 Actor::getColor (void) const
{
  return _color;
}

bool Actor::reflect (void) const
{
  return _reflective;
}

bool Actor::refract (void) const
{
  return _refractive;
}

enum J_MATERIAL Actor::material (void) const
{
  return _material;
}

unsigned long Actor::getId (void) const
{
  return _id;
}

void Actor::setRefl (void)
{
  _reflective = true;
}

void Actor::unsetRefl (void)
{
  _reflective = false;
}

void Actor::setRefr (void)
{
  _refractive = true;
}

void Actor::unsetRefr (void)
{
  _refractive = false;
}

void Actor::setColor (VEC3 color)
{
  _color = color;
}

void Actor::setMaterial (enum J_MATERIAL m)
{
  _material = m;
}

float Actor::materialIndex (enum J_MATERIAL m)
{
  return J_CONSTANTS[m];
}

void Actor::setNormal (VEC3 n)
{
  // Do nothing
}

void Actor::translate (VEC3 t)
{
  MATRIX4 transl;
  transl << 1.0, 0.0, 0.0, t[0],
	    0.0, 1.0, 0.0, t[1],
	    0.0, 0.0, 1.0, t[2],
	    0.0, 0.0, 0.0, 1.0;

  _center = truncate (transl * extend (_center));
}

void Actor::rotate (VEC3 r)
{
  MATRIX3 rx, ry, rz;

  float tx = r[0] * (M_PI / 180.0);
  float ty = r[1] * (M_PI / 180.0);
  float tz = r[2] * (M_PI / 180.0);

  rx << 1.0, 0.0, 0.0,
        0.0, cos (tx), -1.0 * sin (tx),
	0.0, sin (tx), cos (tx);

  ry << cos (ty), 0.0, sin (ty),
        0.0, 1.0, 0.0,
	-1.0 * sin (ty), 0.0, cos (ty);

  rz << cos (tz), -1.0 * sin(tz), 0.0,
        sin (tz), cos (tz), 0.0,
	0.0, 0.0, 1.0;

  _center = rz * ry * rx * _center;
}

void Actor::rotateInPlace (VEC3 r)
{
  MATRIX4 rx, ry, rz, transl, translb;

  float tx = r[0] * (M_PI / 180.0);
  float ty = r[1] * (M_PI / 180.0);
  float tz = r[2] * (M_PI / 180.0);

  rx << 1.0, 0.0, 0.0, 0.0,
        0.0, cos (tx), -1.0 * sin (tx), 0.0,
	0.0, sin (tx), cos (tx), 0.0,
	0.0, 0.0, 0.0, 1.0;

  ry << cos (ty), 0.0, sin (ty), 0.0,
        0.0, 1.0, 0.0, 0.0,
	-1.0 * sin (ty), 0.0, cos (ty), 0.0,
	0.0, 0.0, 0.0, 1.0;

  rz << cos (tz), -1.0 * sin(tz), 0.0, 0.0,
        sin (tz), cos (tz), 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0;

  transl << 1.0, 0.0, 0.0, -1.0 * _center[0],
	    0.0, 1.0, 0.0, -1.0 * _center[1],
	    0.0, 0.0, 1.0, -1.0 * _center[2],
	    0.0, 0.0, 0.0, 1.0;

  translb << 1.0, 0.0, 0.0, _center[0],
	     0.0, 1.0, 0.0, _center[1],
	     0.0, 0.0, 1.0, _center[2],
	     0.0, 0.0, 0.0, 1.0;

  _center = truncate (translb * rz * ry * rx * transl * extend (_center));
}

bool Actor::hasChild (unsigned long index) const
{
  return index == _id;
}
