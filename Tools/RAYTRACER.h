#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <stack>
#include <map>
#include <iostream>
#include <memory>
#include <float.h>

#include "../SETTINGS.h"
#include "../CLASSES.h"
#include "SKELETON.h"
#include "DISPLAYSKELETON.h"
#include "MOTION.h"
#include "QUICKTIME_MOVIE.h"

// Function prototypes

// Vector resizing 
// VEC3 truncate (const VEC4 &);
// VEC4 extend (const VEC3 &);

// PPM management
bool readPPM (const std::string &, unsigned char *&, int &, int &);
void writePPM (const std::string &, int &, int &, const float * &);
void makeFrame (std::string);
void compileMovie (std::string);

// Viewing Transforms
void setLookAt (const VEC3 &, const VEC3 &, const VEC3 &);
void setPerspective (float, float, float);
void setRes (int, int);
void setDepthLimit (int);
void setDepthField (float, float, int);

// Object management
int addObject (std::shared_ptr<Actor>);
void clearObjects (void);

// Lighting management
int addLight (std::shared_ptr<Light>);
void clearLights (void);

// Skeleton management
void loadSkeleton (std::string s, std::string m, int FPS, int start, int frames, MATRIX4 r, VEC3 t);
void trackSkeleton (float t);

// Useful routines
shared_ptr<Actor> makeSphere (float, int, VEC3, VEC3, VEC3);
shared_ptr<Actor> makeSquare (float, float, int, int, VEC3, VEC3);
shared_ptr<Light> makeLight (Actor *, VEC3, float);

#endif
