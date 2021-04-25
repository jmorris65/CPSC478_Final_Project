#include "CUSTOMACTOR.h"

// Bounding Tree implementations
struct boundingTree {
  VEC3 vMin;
  VEC3 vMax;
  unsigned long nItems;
  std::vector<unsigned long> ids;

  int validChildren;
  std::array<std::shared_ptr<BoundingTree>, 8> children;
};

bool rayBoxIntersect (const VEC3 &e, const VEC3 &d,
		      const VEC3 &vMin, const VEC3 &vMax,
		      float tmin, float tmax)
{
  int i;
  VEC3 t0, t1;
  VEC3 invd = VEC3 (1.0 / d[0], 1.0 / d[1], 1.0 / d[2]);

  for (i = 0; i < 3; ++i) {
    t0[i] = (vMin[i] - e[i]) * invd[i];
    t1[i] = (vMax[i] - e[i]) * invd[i];

    if (t0[i] > t1[i]) {
      std::swap (t0[i], t1[i]);
    }
  }

  if ((t0[0] > t1[1]) || (t0[1] > t1[0])) {
    return false;
  }

  if (t0[1] > t0[0])
    t0[0] = t0[1];

  if (t1[1] < t1[0])
    t1[0] = t1[1];

  if ((t0[0] > t1[2]) || (t0[2] > t1[0])) {
    return false;
  }

  if (t0[2] > t0[0])
    t0[0] = t0[2];

  if (t1[2] < t1[0])
    t1[0] = t1[2];

  if (t0[0] > tmax || t1[0] < tmin)
    return false;

  return true;
}

bool triangleInBounds (const std::shared_ptr<BoundingTree> b, const std::shared_ptr<Triangle> t)
{
  // Determine whether or not triangle is inside bounding box
  // DOES NOT WORK
  int i, j;

  std::vector<VEC3> v (3);
  t->getVertices (v[0], v[1], v[2]);

  for (i = 0; i < 3; ++i) {
    j = (i + 1) % 3;
    if (rayBoxIntersect (v[i], (v[j] - v[i]), b->vMin, b->vMax, 0.0, 1.0))
      return true;
  }
  return false;
}

void buildTree (const std::map<unsigned long,std::shared_ptr<Triangle>> &m, std::shared_ptr<BoundingTree> b,
		unsigned maxDepth, unsigned depth)
{
  // Build bounding box tree
  if (depth == maxDepth)
    return;

  int i;

  // Get left and right bounds
  VEC3 range = b->vMax - b->vMin;

  for (i = 0; i < 8; i++) {
    b->children[i].reset (new BoundingTree);
    b->children[i]->nItems = 0;
    b->children[i]->vMin = VEC3 (0.0, 0.0, 0.0);
    b->children[i]->vMax = VEC3 (0.0, 0.0, 0.0);
    b->children[i]->validChildren = 0;
  }
  b->validChildren = 8;

  for (i = 0; i < 8; i++) {
    // Handle x
    if (i > 1 && i < 6) {
      b->children[i]->vMin[0] = b->vMin[0];
      b->children[i]->vMax[0] = b->vMin[0] + (range[0] * 0.5);
    } else {
      b->children[i]->vMin[0] = b->vMin[0] + (range[0] * 0.5);
      b->children[i]->vMax[0] = b->vMax[0];
    }

    // Handle y
    if (i < 4) {
      b->children[i]->vMin[1] = b->vMin[1] + (range[1] * 0.5);
      b->children[i]->vMax[1] = b->vMax[1];
    } else {
      b->children[i]->vMin[1] = b->vMin[1];
      b->children[i]->vMax[1] = b->vMin[1] + (range[1] * 0.5);
    }

    // Handle z
    if (i % 2 == 0) {
      b->children[i]->vMin[2] = b->vMin[2] + (range[2] * 0.5);
      b->children[i]->vMax[2] = b->vMax[2];
    } else {
      b->children[i]->vMin[2] = b->vMin[2];
      b->children[i]->vMax[2] = b->vMin[2] + (range[2] * 0.5);
    }
  }

  for (const int &id : b->ids) {
    for (i = 0; i < 8; i++) {
      if (triangleInBounds (b->children[i], m.at (id))) {
	b->children[i]->ids.push_back (id);
	b->children[i]->nItems = b->children[i]->nItems + 1;
      }
    }
  }

  for (i = 0; i < 8; i++) {
    if (b->children[i]->nItems == 0) {
      b->children[i].reset ();
      b->validChildren = b->validChildren - 1;
    } else {
      buildTree (m, b->children[i], maxDepth, depth + 1);
    }
  }

}

bool getBoundedIntersect (const VEC3 &e, const VEC3 &d,
		const std::map<unsigned long, std::shared_ptr<Triangle>> &m, 
		const std::shared_ptr<BoundingTree> b,
		float &t, const Actor *&c, VEC3 &n)
{
  t = _epsilon;
  c = nullptr;
  n = VEC3 (0.0, 1.0, 0.0);

  float tp;
  const Actor *cp;
  VEC3 np;

  if (!rayBoxIntersect (e, d, b->vMin, b->vMax, 0.0, FLT_MAX))
    return false;

  bool hit = false;
  int i;
  if (b->validChildren) {

    for (i = 0; i < 8; ++i) {
      if (b->children[i] && getBoundedIntersect (e, d, m, b->children[i], tp, cp, np)) {
	if (!hit || tp < t) {
	  t = tp;
	  c = cp;
	  n = np;
	}
	hit = true;
      }
    }

  } else {

    for (const int &id : b->ids) {
      if (m.at(id)->getRayIntersect (e, d, tp, cp, np)) {
	if (!hit || tp < t) {
	  t = tp;
	  c = cp;
	  n = np;
	}
	hit = true;
      }
    }
  }

  return hit;
}

void printBoundingTree (const std::shared_ptr<BoundingTree> b, int depth) {
  // Debugging function
  std::cout << "Depth " << depth << ": " << b->nItems << " Triangles in (" 
       << b->vMin[0] << ", " << b->vMin[1] << ", " << b->vMin[2] << ") - ("
       << b->vMax[0] << ", " << b->vMax[1] << ", " << b->vMax[2] << ")" << std::endl;

  for (int i = 0; i < 8; ++i) {
    if (b->children[i]) {
      printBoundingTree (b->children[i], depth + 1);
    }
  }
}

// Custom Actor implementaions
CustomActor::CustomActor (void)
  : Actor ()
{
  _bounds = std::shared_ptr<BoundingTree> (new BoundingTree);

  _bounds->vMin = VEC3 (FLT_MAX, FLT_MAX, FLT_MAX);
  _bounds->vMax = VEC3 (FLT_MIN, FLT_MIN, FLT_MIN);

  _bounds->nItems = 0;
  _bounds->validChildren = 0;
  _validTree = false;
  _maxDepth = 0;
}

void CustomActor::addShape (int id, std::shared_ptr<Triangle> shape)
{
  int i;
  std::vector<VEC3> v (3);
  shape->getVertices (v[0], v[1], v[2]);

  for (VEC3 &u : v) {
    for (i = 0; i < 3; ++i) {
      _bounds->vMin[i] = std::min ((float) _bounds->vMin[i], (float) u[i]);
      _bounds->vMax[i] = std::max ((float) _bounds->vMax[i], (float) u[i]);
    }
  }

  _bounds->ids.push_back (id);
  _bounds->nItems = _bounds->nItems + 1;
}

void CustomActor::rebuildTree (void) {

  std::map<unsigned long, std::shared_ptr<Triangle>>::iterator it;
  _bounds = std::shared_ptr<BoundingTree> (new BoundingTree);

  _bounds->vMin = VEC3 (FLT_MAX, FLT_MAX, FLT_MAX);
  _bounds->vMax = VEC3 (FLT_MIN, FLT_MIN, FLT_MIN);

  _bounds->nItems = 0;
  _bounds->validChildren = 0;
  _maxDepth = 0;

  for (it = _shapes.begin (); it != _shapes.end (); ++it) {
    addShape (it->first, it->second);
  }

  _maxDepth = (unsigned) floor(log (_bounds->nItems) / log (8));

  buildTree (_shapes, _bounds, _maxDepth, 0);
  _validTree = true;
  // printBoundingTree (_bounds, 0); 
}

void CustomActor::addShapes (std::vector<Triangle *> &shapes)
{
  _validTree = false;

  for (Triangle *shape : shapes) {
    unsigned long id = shape->getId ();
    _shapes[id] = std::shared_ptr<Triangle> (shape);
  }

  rebuildTree ();
}

bool CustomActor::getRayIntersect (VEC3 e, VEC3 d, float &t, const Actor *&c, VEC3 &n) const
{
  bool a = false,
       b;

  float tp;
  const Actor *cp;
  VEC3 np;

  if (_validTree) {

    return getBoundedIntersect (e, d, _shapes, _bounds, t, c, n);

  } else {
    std::map<unsigned long, std::shared_ptr<Triangle>>::const_iterator it;

    for (it = _shapes.begin (); it != _shapes.end (); it++) {
      if ((b = it->second->getRayIntersect (e, d, tp, cp, np))) {
	if (!a || tp < t) {
	  t = tp;
	  c = cp;
	  n = np;
	  a = b;
	}
      }
    }
  }

  return a;
}

std::string CustomActor::toString (void) const
{
  std::stringstream ss;
  ss	    << "CustomActor" << _id
	    << ":\n\tTriangleCount: " << _shapes.size ();

  return ss.str ();
}

VEC3 CustomActor::getNormal (VEC3 p) const
{
  // To be implemented
  return VEC3 (0.0, 1.0, 0.0);
}

int CustomActor::getRandomPoints (int n, std::vector<VEC3> &points) const
{
  std::map<unsigned long, std::shared_ptr<Triangle>>::const_iterator it;
  int add, i, count = 0;
  
  for (i = 0; i < n; ++i) {
    it = _shapes.begin ();
    add = (int) (floor (jrand () * _shapes.size ()));

    std::advance (it, add);
    count = count + it->second->getRandomPoints (1, points);
  }

  return count;
}

void CustomActor::translate (VEC3 t)
{
  _validTree = false;
  std::map<unsigned long, std::shared_ptr<Triangle>>::iterator it;

  for (it = _shapes.begin (); it != _shapes.end (); ++it) {
    it->second->translate (t);
  }

  Actor::translate (t);
  // rebuildTree ();
}

void CustomActor::rotate (VEC3 r)
{
  _validTree = false;
  std::map<unsigned long, std::shared_ptr<Triangle>>::iterator it;

  for (it = _shapes.begin (); it != _shapes.end (); it++) {
    it->second->rotate (r);
  }

  Actor::rotate (r);
  // rebuildTree ();
}

void CustomActor::rotateInPlace (VEC3 r)
{
  _validTree = false;
  std::map<unsigned long, std::shared_ptr<Triangle>>::iterator it;

  for (it = _shapes.begin (); it != _shapes.end(); ++it) {
    it->second->translate (-1.0 * _center);
    it->second->rotate (r);
    it->second->translate (_center);
  }

  // rebuildTree ();
}

bool CustomActor::hasChild (unsigned long index) const
{
  return (index == _id) || (_shapes.find (index) != _shapes.end ());
}
