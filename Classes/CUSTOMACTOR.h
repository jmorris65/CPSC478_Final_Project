#ifndef CUSTOMACTOR_H
#define CUSTOMACTOR_H

#include "ACTOR.h"
#include "TRIANGLE.h"

typedef struct boundingTree BoundingTree;

class CustomActor : public Actor {
  protected:
    std::map<unsigned long, std::shared_ptr<Triangle>> _shapes;
    bool _validTree;
    unsigned _maxDepth;
    std::shared_ptr<BoundingTree> _bounds;
    virtual void addShape (int id, std::shared_ptr<Triangle> shape);

  public:
    // Personal
    CustomActor (void);
    virtual void addShapes (std::vector<Triangle *> &shapes);
    virtual void rebuildTree (void);
    
    // Derived
    virtual bool getRayIntersect (VEC3 e, VEC3 d, float &t, const Actor *&c, VEC3 &n) const override;
    virtual std::string toString (void) const override;
    virtual int getRandomPoints (int n, std::vector<VEC3> &points) const override;
    virtual VEC3 getNormal (VEC3 p) const override;

    virtual void translate (VEC3 t) override;
    virtual void rotate (VEC3 r) override;
    virtual void rotateInPlace (VEC3 r) override;
    virtual bool hasChild (unsigned long index) const override;
};

#endif
