#ifndef POINT_H
#define POINT_H

#include "ACTOR.h"

class Point : public Actor {
  protected:

  public:
    // Personal
    Point (VEC3 coords);

    // Derived
    virtual bool getRayIntersect (VEC3 e, VEC3 d, float &t, const Actor *&c, VEC3 &n) const override;
    virtual VEC3 getNormal (VEC3 p) const override;
    virtual int getRandomPoints (int n, std::vector<VEC3> &points) const override;
    virtual std::string toString (void) const override;
};

#endif
