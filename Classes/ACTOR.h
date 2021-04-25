#ifndef ACTOR_H
#define ACTOR_H

#include <cmath>
#include <iostream>
#include <sstream>
#include <memory>
#include <algorithm>
#include <map>
#include <cfloat>
#include "../SETTINGS.h"
#include "../Tools/JRNG.h"

enum J_MATERIAL {
  J_NONE,
  J_AIR,
  J_GLASS
};

extern const float _epsilon;

VEC3 truncate (const VEC4 &v);
VEC4 extend (const VEC3 &v);
float clamp (float t);

class Actor {
  protected:
    VEC3 _color;
    VEC3 _center;
    bool _reflective;
    bool _refractive;
    enum J_MATERIAL _material;
    static unsigned long _allocated;
    static std::vector<float> J_CONSTANTS;
    unsigned long _id;

  public:
    // Constructor
    Actor (void);

    // Required Implementations
    // Function to return roots
    virtual bool getRayIntersect (VEC3 e, VEC3 d, float &t, const Actor *&c, VEC3 &n) const = 0;
    // Get normal to surface
    virtual VEC3 getNormal (VEC3 p) const = 0;
    // Get random points
    virtual int getRandomPoints (int n, std::vector<VEC3> &points) const = 0;
    // Debugging print out
    virtual std::string toString (void) const = 0;
    
    // Translation function
    virtual void translate (VEC3 t);

    // Rotation functions
    virtual void rotate (VEC3 r);
    virtual void rotateInPlace (VEC3 r);


    // Fully implemented optional overwrite in special cases
    // Function to get color
    virtual VEC3 getColor (void) const;

    // Getters
    virtual bool reflect (void) const;
    virtual bool refract (void) const;

    virtual enum J_MATERIAL material (void) const;
    virtual unsigned long getId (void) const;

    virtual bool hasChild (unsigned long) const;

    // Setters
    virtual void setRefl (void);
    virtual void unsetRefl (void);
    
    virtual void setRefr (void);
    virtual void unsetRefr (void);
    
    virtual void setColor (VEC3 color);

    virtual void setMaterial (enum J_MATERIAL material);
    virtual void setNormal (VEC3 n);

    // Static
    static float materialIndex (enum J_MATERIAL m);
};

#endif
