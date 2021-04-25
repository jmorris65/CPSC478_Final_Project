#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "ACTOR.h"

class Triangle : public Actor {
  protected:
    VEC3 _a;
    VEC3 _b;
    VEC3 _c;
    VEC3 _normal;

  public:
    // Personal
    Triangle (VEC3 a, VEC3 b, VEC3 c);
    Triangle (VEC3 a, VEC3 b, VEC3 c, VEC3 n);
    void setNormal (VEC3 n);
    void getVertices (VEC3 &a, VEC3 &b, VEC3 &c) const;

    // Derived
    virtual bool getRayIntersect (VEC3 e, VEC3 d, float &t, const Actor *&c, VEC3 &n) const override;
    virtual VEC3 getNormal (VEC3 p) const override;
    virtual int getRandomPoints (int n, std::vector<VEC3> &points) const override;
    virtual std::string toString (void) const override;

    virtual void translate (VEC3 t) override;
    virtual void rotate (VEC3 r) override;
};

#endif
