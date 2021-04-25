#ifndef CYLINDER_H
#define CYLINDER_H

#include "ACTOR.h"

class Cylinder : public Actor {
  protected:
    float _radius;
    float _height;
    MATRIX4 _rotation;

  public:
    // Personal
    Cylinder (VEC3 lcenter, VEC3 rcenter, float radius);

    // Derived
    virtual bool getRayIntersect (VEC3 e, VEC3 d, std::pair<float, const Actor *> &v) const override;
    virtual VEC3 getNormal (VEC3 p) const override;
    virtual int getRandomPoints (int n, std::vector<VEC3> &points) const override;
    virtual std::string toString (void) const override;

    virtual void rotate (VEC3 r) override;
};

#endif
