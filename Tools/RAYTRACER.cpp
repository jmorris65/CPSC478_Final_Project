#include "RAYTRACER.h"

using namespace std;

// Image globals

// Basis
static VEC3 _e = VEC3 (0.0, 0.0, 0.0), 
	    _u = VEC3 (1.0, 0.0, 0.0),
	    _v = VEC3 (0.0, 1.0, 0.0),
	    _w = VEC3 (0.0, 0.0, -1.0);

// Dimensions
static int _x = 500,
	   _y = 500;

// Perspective
static float _l = -1.0,
	     _r = 1.0,
	     _b = -1.0,
	     _t = 1.0,
	     _n = 1.0;

// Object array
static map<unsigned long, shared_ptr<Actor>> _objects;

// Stick-Man classes
static vector<DisplaySkeleton> _displayers;
static Skeleton * _skeleton;
static Motion * _motion;
static bool _loadedSkeleton;
static bool _track;
static float _trackDistance;
static vector<int> _currentFrames;
static vector<int> _FPSs;
static vector<int> _ends;
static int skeletonLimit = 2;

static vector<MATRIX4> _rEdits;
static vector<VEC4> _tEdits;

// Lights array
static map<unsigned long, shared_ptr<Light>> _lights;

// Epsilon for shadows
const float _epsilon = 0.001;

// Depth
static int _depthLimit = 10;

// Distributions
static int _shadowDistro = 1;

static bool _depthField;
static int _depthDistro = 1;
static float _focalLength = 1.0;
static float _aperture = 0.0;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/* 
VEC3 truncate (const VEC4 &v)
{
  return VEC3 (v[0], v[1], v[2]);
}

VEC4 extend (const VEC3 &v)
{
  return VEC4 (v[0], v[1], v[2], 1.0);
}
*/

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
bool readPPM(const string& filename, unsigned char *&pixels, int& xRes, int& yRes)
{
  // try to open the file
  FILE *fp;
  fp = fopen(filename.c_str(), "rb");
  if (fp == NULL)
  {
    cout << " Could not open file \"" << filename.c_str() << "\" for reading." << endl;
    return false;
  }

  // get the dimensions
  unsigned char newline;
  fscanf(fp, "P6\n%d %d\n255%c", &xRes, &yRes, &newline);
  if (newline != '\n') {
    cout << " The header of " << filename.c_str() << " may be improperly formatted." << endl;
    cout << " The program will continue, but you may want to check your input. " << endl;
  }
  int totalCells = xRes * yRes;

  // grab the pixel values
  pixels = new unsigned char[3 * totalCells];
  fread(pixels, 1, totalCells * 3, fp);

  // clean up
  fclose(fp);
  cout << " Read in file " << filename.c_str() << endl;
  return true;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void writePPM(const string& filename, int& xRes, int& yRes, const float* values)
{
  int totalCells = xRes * yRes;
  unsigned char* pixels = new unsigned char[3 * totalCells];
  for (int i = 0; i < 3 * totalCells; i++)
    pixels[i] = values[i];

  FILE *fp;
  fp = fopen(filename.c_str(), "wb");
  if (fp == NULL)
  {
    cout << " Could not open file \"" << filename.c_str() << "\" for writing." << endl;
    cout << " Make sure you're not trying to write from a weird location or with a " << endl;
    cout << " strange filename. Bailing ... " << endl;
    exit(0);
  }

  fprintf(fp, "P6\n%d %d\n255\n", xRes, yRes);
  fwrite(pixels, 1, totalCells * 3, fp);
  fclose(fp);
  delete[] pixels;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void setLookAt (const VEC3 &e, const VEC3 &look, const VEC3 &up) {
  // Sets basis vectors
  VEC3 g,
       u, v, w;

  g = look - e;
  w = -1.0 * g / sqrt (g.dot (g));
  u = up.cross (w);
  u = u / sqrt (u.dot (u));
  v = w.cross (u);

  _e = e;
  _u = u;
  _v = v;
  _w = w;
}

void setPerspective (float n, float fovy, float aspect) {
  // Sets viewing frustum
  float l, r, t, b;

  fovy = fovy * M_PI / 180.0;
  t = n * tan (fovy / 2.0);
  b = -1.0 * t;
  r = t * aspect;
  l = -1.0 * r;

  _l = l;
  _r = r;
  _b = b;
  _t = t;
  _n = n;
}

void setRes (int x, int y)
{
  _x = x;
  _y = y;
}

void setDepthLimit (int d)
{
  _depthLimit= d;
}

void setDepthField (float a, float f, int distro)
{
  _focalLength = f;
  _depthDistro = distro;

  _aperture = (a * _n) / f;
  _aperture = a - _aperture;

  _depthField = true;
}

void generateRay (int i, int j, VEC3 &o, VEC3 &d)
{
  // Uses pixel resolution and viewing frustum to build a ray
  VEC3 s;
  float u, v;

  u = _l + (_r - _l) * ((float) i + 0.5) / _x;
  v = _b + (_t - _b) * ((float) j + 0.5) / _y;

  s = (u * _u) + (v * _v) - (_n * _w);
  o = _e + s;

  s = s / sqrt (s.dot (s));
  d = s;
}

void generateDepthRay (const VEC3 &o, const VEC3 &d, VEC3 &oJitter, VEC3 &dJitter)
{
  VEC3 p = _e - (_w * _focalLength);
  VEC3 n = -1.0 * _w;

  float den = n.dot (d);
  if (den > 0.0) {
    float t = (p - o).dot (n);
    t = t / den;
    VEC3 intersect = o + (d * t);

    float uS = jrand () * _aperture;
    float vS = jrand () * _aperture;

    oJitter = o + uS * _u + vS * _v;
    
    dJitter = intersect - oJitter;
    dJitter = dJitter / sqrt (dJitter.dot (dJitter));
  }
}

VEC3 shadeRay (VEC3 p, VEC3 d, VEC3 n, const Actor *c) {
  // Interative and helper variables
  int i, j,
      rays,
      count = 0;

  // Hold lights and objects in scene
  map<unsigned long, shared_ptr<Actor>>::iterator actorIt;
  map<unsigned long, shared_ptr<Light>>::iterator lightIt;
  shared_ptr<Light> l;
  shared_ptr<Actor> a;

  // Hold object intersection results
  float tp;
  const Actor *cp;

  // Get vectors and colors important to shading computations
  VEC3 np, r, e = -1.0 * d,
       scolor = c->getColor (),
       lcolor,
       kcolor;

  // Final color
  VEC3 color (0.0, 0.0, 0.0);

  // Vector to store shadow rays
  vector<VEC3> shadowRays;
  vector<float> intensities;
  shadowRays.reserve (_shadowDistro);
  intensities.reserve (_shadowDistro);

  for (lightIt = _lights.begin (); lightIt != _lights.end (); lightIt++) {

    // Get next light in list and possible color at pixel
    l = lightIt->second;

    if (l->hasChild (c->getId ())) {
      color = color + (c->getColor () * l->getIntensity ());
      continue;
    }

    // Get shadow rays
    rays = l->getShadowRays (p, _shadowDistro, shadowRays, intensities);

    // Iterate thought all shadow rays 
    for (i = 0; i < rays; ++i) {

      // If producing shadows iterate through other objects
      for (actorIt = _objects.begin (); actorIt != _objects.end (); actorIt++) {
	a = actorIt->second;

	// If another object is hit, ray is invalid
	if (a->getRayIntersect (p, shadowRays[i], tp, cp, np) 
	    && tp < intensities[i]
	    && !(l->hasChild (cp->getId ()))) {
	  break;
	}
      }

      // If no objects were hit then shade pixel
      if (actorIt == _objects.end ()) {
	// Compute and add Lambertian shading
        lcolor = l->getColor ();
        kcolor = VEC3 (lcolor[0] * scolor[0], lcolor[1] * scolor[1], lcolor[2] * scolor[2]);
	color = color + ((max ((float) 0.0, (float) n.dot (shadowRays[i])) * (1.0 / (float) rays) * clamp (1.0 / intensities[i])) * kcolor);

	// Compute and add Phong shading
	r = -1.0 * shadowRays[i] + (2.0 * (float) n.dot (shadowRays[i])) * n;
	r = r / sqrt (r.dot (r));
	color = color + ((pow (max ((float) 0.0, (float) e.dot (r)), 10) * (1.0 / (float) rays) * clamp (1.0 / intensities[i])) * kcolor);
      }
    }
  }

  return color;
}

VEC3 traceRay (VEC3 e, VEC3 d, int depth) {
  // Initialize color vector
  VEC3 color (0.0, 0.0, 0.0),
       p, n, np, r;

  // Depth limit reached return black
  if (depth == 0)
    return color;

  // Iterative and storage variables
  
  int i;
  unsigned long id;
  enum J_MATERIAL matIn, matOut;
  float best = _epsilon, tp,
	rad, nonrad, nIn, nOut,
	R0, kRefl, kRefr;

  map<unsigned long, shared_ptr<Actor>>::iterator actorIt;
  
  shared_ptr<Actor> a;
  const Actor *c = nullptr, *cp;

  for (actorIt = _objects.begin (); actorIt != _objects.end (); actorIt++) {
    // Get next object in vector
    a = actorIt->second;

    // Get intersect
    if (a->getRayIntersect (e, d, tp, cp, np)) {
      if (best == _epsilon || tp < best) {
	best = tp;
	id = actorIt->first;
	c = cp;
	n = np;
      }
    }
  }

  // Valid object was hit
  if (best > _epsilon) {
    
    // Compute contact point
    p = e + (d * best);

    matIn = J_AIR;
    matOut = _objects[id]->material ();

    kRefl = 1.0;
    kRefr = 1.0;

    if (n.dot (d) > 0.0) {
      // Inside object
      swap (matIn, matOut);
      n = -1.0 * n;
    }

    nIn = Actor::materialIndex (matIn);
    nOut = Actor::materialIndex (matOut);

    // Get Fresnel (Using Schlick)
    if (c->refract ()) {
      R0 = (nOut - 1.0) / (nOut + 1.0);
      R0 = R0 * R0;

      kRefl = R0 + (1.0 - R0) * (pow (1 - (-1.0 * d).dot (n), 5));
      kRefr = 1 - kRefl;
    }
 
    if (c->reflect ()) {
      // Compute reflected ray 
      r = d - (2.0 * (float) d.dot (n)) * n;
      r = r / sqrt (r.dot (r));
      color = color + kRefl * traceRay (p, r, depth - 1);
    }

    if (c->refract ()) {
      // Compute refracted ray  
      rad = 1 - (((nIn * nIn) * (1.0 - (d.dot(n) * d.dot(n)))) / (nOut * nOut));
      if (rad >= 0.0) {
	nonrad = nIn / nOut;
        r = nonrad * (d + n * ((-1.0 * d).dot (n))) - sqrt (rad) * n;
        r = r / sqrt (r.dot (r));
	
	color = color + kRefr * traceRay (p, r, depth - 1);
      }
    }
    
    // Compute object color
    color = color + shadeRay (p, d, n, c);
  }

  color[0] = (color[0] > 1.0) ? 1.0 : color[0];
  color[1] = (color[1] > 1.0) ? 1.0 : color[1];
  color[2] = (color[2] > 1.0) ? 1.0 : color[2];

  return color;
}

void setSkeletonToFrame (int n)
{
  if (n >= _displayers.size ()) {
    exit (1);
  }

  // Code originally in previz.cpp
  if (_currentFrames[n] < 0) {
    exit (1);
  }

  if (_displayers[n].GetSkeletonMotion (0) != nullptr) {
    int postureId;
    
    if (_currentFrames[n] >= _displayers[n].GetSkeletonMotion (0)->GetNumFrames ()) {
      postureId = _displayers[n].GetSkeletonMotion (0)->GetNumFrames () - 1;
    
    } else if (_currentFrames[n] > _ends[n]) {
      _currentFrames[n] = _ends[n];
      postureId = _ends[n];
    
    } else {
      postureId = _currentFrames[n];
    }

    _displayers[n].GetSkeleton (0)->setPosture (* (_displayers[n].GetSkeletonMotion (0)->GetPosture (postureId)));
  }
}

VEC3 addSkeletonToFrame (int n)
{
  setSkeletonToFrame (n);
  _displayers[n].ComputeBonePositions (DisplaySkeleton::BONES_AND_LOCAL_FRAMES);

  bool usingSpheres = false;
  int i, j;

  vector<MATRIX4> &rotations = _displayers[n].rotations ();
  vector<MATRIX4> &scalings = _displayers[n].scalings ();
  vector<VEC4> &translations = _displayers[n].translations ();
  vector<float> &lengths = _displayers[n].lengths ();

  int totalBones = rotations.size ();

  VEC3 skeletonEye;

  for (int i = 1; i < totalBones; i++) {
    MATRIX4 &rotation = rotations[i];
    MATRIX4 &scaling = scalings[i];
    VEC4 &translation = translations[i];

    VEC4 leftVertex (0.0, 0.0, 0.0, 1.0);
    VEC4 rightVertex (0.0, 0.0, lengths[i], 1.0);

    leftVertex = (_rEdits[n] * rotation) * scaling * leftVertex + (_tEdits[n] + (_rEdits[n] * translation));
    rightVertex = (_rEdits[n] * rotation) * scaling * rightVertex + (_tEdits[n] + (_rEdits[n] * translation));

    if (i == 1) {
      skeletonEye = translation.head<3> ();
    }

    if (usingSpheres) {
      VEC3 direction = (rightVertex - leftVertex).head<3>();
      float magnitude = direction.norm ();
      direction *= 1.0 / magnitude;

      float sphereRadius = 0.05;
      int totalSpheres = magnitude / (2.0 * sphereRadius);
      float rayIncrement = magnitude / (float) totalSpheres;
      VEC3 color (1.0, 1.0, 1.0);
      VEC3 rot (0.0, 0.0, 0.0);

      // makeSphere (sphereRadius, 1, color, leftVertex.head<3> (), rot);
      // makeSphere (sphereRadius, 1, color, rightVertex.head<3> (), rot);
      shared_ptr<Actor> a;
      a.reset (new Sphere (leftVertex.head<3> (), sphereRadius));
      a->setColor (color);
      addObject (a);
      
      a.reset (new Sphere (rightVertex.head<3> (), sphereRadius));
      a->setColor (color);
      addObject (a);

      for (j = 0; j < totalSpheres; j++) {
	VEC3 cent = ((float) j + 0.5) * rayIncrement * direction + leftVertex.head<3> ();
	// makeSphere (sphereRadius, 1, color, cent, rot);
	a.reset (new Sphere (cent, sphereRadius));
	a->setColor (color);
	addObject (a);
      }
    } else {
      shared_ptr<Actor> a;
      a.reset (new Cylinder (leftVertex.head<3> (), rightVertex.head<3> (), 0.05));
      a->setColor (VEC3 (1.0, 1.0, 1.0));
      addObject (a);
    }
  }

  _currentFrames[n] = _currentFrames[n] + _FPSs[n];

  return skeletonEye;
}

void makeFrame (string filename) {
  int numPixels = _x * _y,
      i, j, x, y;

  VEC3 skeletonEye;

  if (_loadedSkeleton) {
    skeletonEye = addSkeletonToFrame (0);
    for (i = 1; i < _displayers.size (); ++i) {
      addSkeletonToFrame (i);
    }
  }

  if (_track) {
    VEC3 shift (0.5, 0.0, 0.0);
    VEC3 newEye = skeletonEye + VEC3 (_trackDistance, 0.0, -1.0 * _trackDistance);

    newEye = newEye + shift;
    skeletonEye = skeletonEye + shift;

    setLookAt (newEye, skeletonEye, _v);
  }

  float *pixels = (float *) malloc (3 * numPixels * sizeof (float));

  for (y = _y - 1, i = 0; y > -1; y--) {

    for (x = 0; x < _x; x++) {

      VEC3 o, d;
      generateRay (x, y, o, d);

      VEC3 color (0.0, 0.0, 0.0);

      if (_depthField) {
	VEC3 oJitter, dJitter;
	for (j = 0; j < _depthDistro; ++j) {
	  generateDepthRay (o, d, oJitter, dJitter);
	  VEC3 col = traceRay (oJitter, dJitter, _depthLimit);
	  color = color + (col / _depthDistro);
	}

      } else {
	color = traceRay (o, d, _depthLimit);

      }

      pixels[3 * i] = 255.0 * color[0];
      pixels[3 * i + 1] = 255.0 * color[1];
      pixels[3 * i + 2] = 255.0 * color[2];

      i = i + 1;
    }
  }

  writePPM (filename, _x, _y, pixels);
  free (pixels);
}

int addObject (std::shared_ptr<Actor> a)
{
  int id = a->getId ();
  _objects[id] = a;
  return id;
}

int addLight (std::shared_ptr<Light> l)
{
  int id = l->getId ();
  _lights[id] = l;
  return id;
}

void clearObjects (void)
{
  _objects.clear ();
}

void clearLights (void)
{
  _lights.clear ();
}

void loadSkeleton (std::string s, std::string m, int FPS, int start, int frames, MATRIX4 r, VEC3 t)
{
  if (!_loadedSkeleton) {
    _displayers.reserve (skeletonLimit);
    _displayers.clear ();

    _FPSs.reserve (skeletonLimit);
    _FPSs.clear ();

    _currentFrames.reserve (skeletonLimit);
    _currentFrames.clear ();

    _ends.reserve (skeletonLimit);
    _ends.clear ();

    _rEdits.reserve (skeletonLimit);
    _rEdits.clear ();

    _tEdits.reserve (skeletonLimit);
    _tEdits.clear ();
  }

  int n = _displayers.size ();
  if (n == skeletonLimit) {
    return;
  }

  _displayers.push_back (DisplaySkeleton ());

  // Code originally in previz.cpp
  // Load skeleton
  _skeleton = new Skeleton (s.c_str (), MOCAP_SCALE);
  _skeleton->setBasePosture ();
  _displayers[n].LoadSkeleton (_skeleton);
  

  // Load motion
  _motion = new Motion (m.c_str (), MOCAP_SCALE, _skeleton);
  _displayers[n].LoadMotion (_motion);
  _skeleton->setPosture (*(_displayers[n].GetSkeletonMotion (0)->GetPosture (0)));

  // State variables
  _FPSs.push_back (FPS);
  _currentFrames.push_back (start);
  _ends.push_back (start + (frames * FPS));
  _loadedSkeleton = true;

  // Alterations
  _rEdits.push_back (r);
  _tEdits.push_back (extend (t));

}

void trackSkeleton (float t) {
  _track = true;
  _trackDistance = t;
}

void compileMovie (std::string root)
{
  QUICKTIME_MOVIE movie;

  char filename[256];
  bool readSuccess = true;
  int frameNumber = 0,
      width, height;
  unsigned char *pixels = nullptr;

  while (readSuccess)
  {
    sprintf (filename, "./Frames/%s.%04d.ppm", root.c_str(), frameNumber);

    if ((readSuccess = readPPM (filename, pixels, width, height))) {
      movie.addFrame (pixels, width, height);
    }

    if (pixels) {
      delete[] pixels;
      pixels = nullptr;
    }

    frameNumber = frameNumber + 1;
  }

  sprintf (filename, "%s.mov", root.c_str());
  movie.writeMovie (filename);
}


// Useful routines

shared_ptr<Actor> makeSquare (float l, float w, int row, int col, VEC3 transl, VEC3 rot)
{
  float r, c, h = 0.0;

  int i, j;

  CustomActor *s = new CustomActor ();

  vector<VEC3> vertices, normals;
  vector<VEC3I> tris;

  for (i = 0; i < col + 1; ++i) {
    c = (-1.0 * (w * 0.5)) + ((float) i * (w / (float) col));

    for (j = 0; j < row + 1; ++j) {
      r = (-1.0 * (l * 0.5)) + ((float) j * (l / (float) row));
      vertices.push_back (VEC3 (c, h, r));
    }
  }

  normals.reserve (2 * row * col);
  tris.reserve (2 * row * col);

  for (i = 0; i < col; ++i) {
    for (j = 0; j < row; ++j) {

      int topl = (i * (row + 1)) + j,
	  topr = ((i + 1) * (row + 1)) + j,
	  botl = (i * (row + 1)) + (j + 1),
	  botr = ((i + 1) * (row + 1)) + (j + 1);

      VEC3 v0 = vertices[topl] - vertices[topr],
           v1 = vertices[botl] - vertices[topr],
	   v2 = vertices[botr] - vertices[topr];

      tris.push_back (VEC3I (topl, topr, botl));
      normals.push_back (v0.cross (v1).normalized ());

      tris.push_back (VEC3I (topr, botr, botl));
      normals.push_back (v1.cross (v2).normalized ());
    }
  }

  vector<Triangle *> triangles;

  for (i = 0; i < 2 * row * col; i = i + 2) {

    triangles.push_back (new Triangle (vertices[tris[i][0]],
				       vertices[tris[i][1]],
				       vertices[tris[i][2]],
				       normals[i]));
    triangles[i]->setColor (VEC3 (1.0, 0.0, 0.0));

    triangles.push_back (new Triangle (vertices[tris[i + 1][0]],
				       vertices[tris[i + 1][1]],
				       vertices[tris[i + 1][2]],
				       normals[i + 1]));

    triangles[i + 1]->setColor (VEC3 (0.0, 1.0, 0.0));
  }

  s->addShapes (triangles);
  s->rotate (rot);
  s->translate (transl);
  s->rebuildTree ();

  shared_ptr<Actor> sp = shared_ptr<Actor> (s);
  addObject (sp);
  return sp;
}

shared_ptr<Actor> makeSphere (float radius, int subdivisions, VEC3 color, VEC3 transl, VEC3 rot)
{
  CustomActor *s;

  s = new CustomActor ();

  vector<VEC3> vertices, normals;
  vector<VEC3I> tris, temp;

  int i, j, v;

  vertices.push_back (VEC3 (-1.0 * radius, 0.0, 0.0));
  vertices.push_back (VEC3 (radius, 0.0, 0.0));
  vertices.push_back (VEC3 (0.0, -1.0 * radius, 0.0));
  vertices.push_back (VEC3 (0.0, radius, 0.0));
  vertices.push_back (VEC3 (0.0, 0.0, -1.0 * radius));
  vertices.push_back (VEC3 (0.0, 0.0, radius));

  tris.push_back (VEC3I (0, 3, 5));
  tris.push_back (VEC3I (5, 3, 1));
  tris.push_back (VEC3I (1, 3, 4));
  tris.push_back (VEC3I (4, 3, 0));
  tris.push_back (VEC3I (0, 2, 5));
  tris.push_back (VEC3I (5, 2, 1));
  tris.push_back (VEC3I (1, 2, 4));
  tris.push_back (VEC3I (4, 2, 0));

  normals.reserve (tris.size ());
  for (i = 0; i < tris.size (); ++i) {
    normals[i] = VEC3 (0.0, 0.0, 0.0);
    for (j = 0; j < 3; j++) {
      normals[i] = normals[i] + vertices[tris[i][j]];
    }
    normals[i] = normals[i].normalized ();
  }

  while (subdivisions--) {
    for (i = 0; i < tris.size (); ++i) {
      v = vertices.size ();

      vertices.push_back ((vertices[tris[i][0]] + vertices[tris[i][1]]).normalized () * radius);
      vertices.push_back ((vertices[tris[i][1]] + vertices[tris[i][2]]).normalized () * radius);
      vertices.push_back ((vertices[tris[i][2]] + vertices[tris[i][0]]).normalized () * radius);

      temp.push_back (VEC3I (tris[i][0], v, v + 2));
      temp.push_back (VEC3I (tris[i][1], v, v + 1));
      temp.push_back (VEC3I (tris[i][2], v + 1, v + 2));
      temp.push_back (VEC3I (v, v + 1, v + 2));
    }

    tris.clear ();
    swap (tris, temp);

    normals.reserve (tris.size ());
    for (i = 0; i < tris.size (); ++i) {
      normals[i] = VEC3 (0.0, 0.0, 0.0);
      for (j = 0; j < 3; ++j) {
	normals[i] = normals[i] + vertices[tris[i][j]];
      }
      normals[i] = normals[i].normalized ();
    }

  }

  vector<Triangle *> triangles;
  triangles.reserve (tris.size ());
  triangles.clear ();

  for (i = 0; i < tris.size (); ++i) {
    triangles.push_back (new Triangle (vertices[tris[i][0]],
				       vertices[tris[i][1]],
				       vertices[tris[i][2]],
				       normals[i]));
    triangles[i]->setColor (color);
  }

  s->addShapes (triangles);
  s->rotate (rot);
  s->translate (transl);
  s->rebuildTree ();


  shared_ptr<Actor> sp (s);
  shared_ptr<Light> lp (new Light (sp, color, 1.0));
  addObject (lp);
  addLight (lp);
  return lp;
}
