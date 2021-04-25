#ifndef SPHERE_H
#define SPHERE_H

#include "ACTOR.h"

class Sphere : public Actor {
  protected:
    float _radius;

  public:
    // Personal
    Sphere (VEC3 center, float radius);

    // Derived
    virtual bool getRayIntersect (VEC3 e, VEC3 d, float &t, const Actor *&c, VEC3 &n) const override;
    virtual std::string toString (void) const override;
    virtual VEC3 getNormal (VEC3 p) const override;
    virtual int getRandomPoints (int n, std::vector<VEC3> &points) const override;
};

#endif
