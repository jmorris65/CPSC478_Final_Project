#include "CYLINDER.h"

// Initialization

// Implementations

Cylinder::Cylinder (VEC3 lcenter, VEC3 rcenter, float radius)
  : _radius {radius},
    Actor ()
{
  _center = lcenter;
  rcenter = rcenter - lcenter;

  MATRIX4 rx = MATRIX4::Identity (), ry = MATRIX4::Identity (), rz = MATRIX4::Identity ();


  if (rcenter[1] != 0.0) {
    float tx = (M_PI / 2.0) - atan (rcenter[2] / rcenter[1]);
    rx << 1.0, 0.0, 0.0, 0.0,
          0.0, cos (tx), -1.0 * sin (tx), 0.0,
	  0.0, sin (tx), cos (tx), 0.0,
	  0.0, 0.0, 0.0, 1.0;
    rcenter = truncate (rx * extend (rcenter));
  }
  
  if (rcenter[0] != 0.0) {
    float ty = (M_PI / 2.0) - atan (rcenter[2] / rcenter[0]);
    ty = -1.0 * ty;
    ry << cos (ty), 0.0, sin (ty), 0.0,
          0.0, 1.0, 0.0, 0.0,
	  -1.0 * sin (ty), 0.0, cos (ty), 0.0,
	  0.0, 0.0, 0.0, 1.0;
    rcenter = truncate (ry * extend (rcenter));
  }

  if (rcenter[2] < 0.0) {
    float tz = M_PI;
    rz << cos (tz), 0.0, sin (tz), 0.0,
          0.0, 1.0, 0.0, 0.0,
	  -1.0 * sin (tz), 0.0, cos (tz), 0.0,
	  0.0, 0.0, 0.0, 1.0;
    rcenter = truncate (rz * extend (rcenter));
  }

  // std::cout << "\n\nrcenter = " << rcenter << "\n\n" << std::endl;

  _height = rcenter[2];
  _rotation = rz * ry * rx;
}

bool Cylinder::getRayIntersect (VEC3 e, VEC3 d, float &t, const Actor *&c, VEC3 &n) const
{
  t = _epsilon;
  c = nullptr;
  n = VEC3 (0.0, 1.0, 0.0);

  e = truncate (_rotation * extend (e - _center));
  d = truncate (_rotation * extend (d));

  float A = (d[0] * d[0]) + (d[1] * d[1]),
	B = (2.0 * e[0] * d[0]) + (2.0 * e[1] * d[1]),
	C = (e[0] * e[0]) + (e[1] * e[1]) - (_radius * _radius);

  float radical = (B * B) - (4.0 * A * C);

  if (radical < 0.0)
    return false;

  float t0 = ((-1.0 * B) - sqrt (radical)) / (2.0 * A);
  VEC3 contact;

  if (t0 > _epsilon) {
    contact = e + (t0 * d);
    if (contact[2] >= 0.0 && contact[2] <= _height) {
      t = t0;
      c = this;
      n = truncate (_rotation.transpose () * extend (contact - VEC3 (0.0, 0.0, contact[2]))).normalized ();
      return true;
    }
  }

  float invdz = 1.0 / d[2];

  float tcap0 = (0.0 - e[2]) * invdz,
	tcap1 = (_height - e[2]) * invdz;

  float dir0 = -1.0,
	dir1 = 1.0;

  if (tcap0 > tcap1) {
    std::swap (tcap0, tcap1);
    std::swap (dir0, dir1);
  }

  if (tcap0 > _epsilon) {
    contact = e + (tcap0 * d);

    if ((contact[0] * contact[0]) + (contact[1] * contact[1]) <= (_radius * _radius)) {
      t = tcap0;
      c = this;
      n = truncate (_rotation.transpose () * VEC4 (0.0, 0.0, dir0, 1.0)).normalized ();
      return true;
    }
  }

  if (tcap1 > _epsilon) {
    contact = e + (tcap1 * d);
    
    if ((contact[0] * contact[0]) + (contact[1] * contact[1]) <= (_radius * _radius)) {
      t = tcap0;
      c = this;
      n = truncate (_rotation.transpose () * VEC4 (0.0, 0.0, dir1, 1.0)).normalized ();
      return true;
    }
  }


  float t1 = ((-1.0 * B) + sqrt (radical)) / (2.0 * A);

  if (t1 > _epsilon) {
    contact = e + (t1 * d);
    if (contact[2] >= 0.0 && contact[2] <= _height) {
      t = t1;
      c = this;
      n = truncate (_rotation.transpose () * extend (contact - VEC3 (0.0, 0.0, contact[2])));
      return true;
    }
  }

  return false;
}

VEC3 Cylinder::getNormal (VEC3 p) const
{
  p = truncate (_rotation * extend (p));

  VEC3 n = p - VEC3 (0.0, 0.0, p[2]);

  return truncate (_rotation.transpose () * extend (n));
}

int Cylinder::getRandomPoints (int n, std::vector<VEC3> &points) const
{
  int i = 0;
  float x, y, z;
  VEC3 p;
  for (i = 0; i < n; ++i) {

    x = jrand () * _radius;
    y = sqrt ((_radius * _radius) - (x * x));
    z = jrand () * _height;

    p = VEC3 (x, y, z);
    points.push_back (truncate (_rotation.transpose () * extend (p)));
  }
  return i;
}

std::string Cylinder::toString (void) const
{
  std::stringstream ss;
  ss	    << "Cylinder" << _id
	    << ":\n\t" << "Center at: ("
	    << _center[0] << ", "
	    << _center[1] << ", "
	    << _center[2] << ")";

  return ss.str ();
}

void Cylinder::rotate (VEC3 r)
{
  float tx = r[0],
	ty = r[1],
	tz = r[2];

  MATRIX4 rx, ry, rz;

  rx << 1.0, 0.0, 0.0, 0.0,
        0.0, cos (tx), -1.0 * sin (tx), 0.0,
	0.0, sin (tx), cos (tx), 0.0,
	0.0, 0.0, 0.0, 1.0;

  ry << cos (ty), 0.0, sin (ty), 0.0,
	0.0, 1.0, 0.0, 0.0,
	-1.0 * sin (ty), 0.0, cos (ty), 0.0,
	0.0, 0.0, 0.0, 1.0;

  rz << cos (tz), -1.0 * sin (tz), 0.0, 0.0,
	sin (tz), cos (tz), 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0;

  MATRIX all = rz * ry * rx;

  _center = truncate (all * extend (_center));
  _rotation = _rotation * all.transpose ();
}
