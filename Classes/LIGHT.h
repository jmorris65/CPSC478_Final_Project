#ifndef LIGHT_H
#define LIGHT_H

#include "ACTOR.h"

class Light : public Actor {
  protected:
    std::shared_ptr<Actor> _shape;
    float _intensity;

  public:
    // Personal
    Light (Actor *shape, VEC3 color, float intensity);
    int getShadowRays (VEC3 p, int n, std::vector<VEC3> &v, std::vector<float> &j) const;

    // Derived
    virtual bool getRayIntersect (VEC3 e, VEC3 d, float &t, const Actor *&c, VEC3 &n) const override;
    virtual std::string toString (void) const override;
    virtual int getRandomPoints (int n, std::vector<VEC3> &points) const override;
    virtual VEC3 getNormal (VEC3 p) const override;

    virtual bool hasChild (unsigned long index) const override;
    virtual VEC3 getColor (void) const override;
};

#endif
