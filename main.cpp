#include "MAIN.h"

using namespace std;

void addFloor (void);
void addWalls (void);
void addLights (int);
void movie (void);
void test (void);

int main (int argc, char **argv)
{
  movie ();
  // test ();
  return EXIT_SUCCESS;
}

void test (void)
{
  char name[256];
  VEC3 eye (0.0, 2.0, 8.0);
  VEC3 lookAt (0.0, 1.0, 0.0);
  VEC3 u (0.0, 1.0, 0.0);

  int x = 500;
  int y = 500;
  int depthLimit = 10;

  setLookAt (eye, lookAt, u);
  setPerspective (1.0, 65.0, (float) x / (float) y);
  setRes (x, y);
  setDepthLimit (depthLimit);
  // setDepthField (0.15, 10.0, 10);

  clearObjects ();
  clearLights ();

  Actor *a;

  addFloor ();

  // makeSphere (0.2, 0, VEC3 (0.75, 0.75, 0.75), VEC3 (2.0, 3.0, 2.0), VEC3::Zero ());
  
  // a = new Sphere (VEC3 (2.0, 3.0, 2.0), 0.2);
  // a->setColor (VEC3 (0.0, 0.3, 0.0));
  // makeLight (a, VEC3 (1.0, 1.0, 1.0), 1.0);

  a = new Sphere (VEC3 (0.0, 2.0, 0.0), 1.0);
  a->setColor (VEC3 (0.0, 0.0, 1.0));
  addObject (shared_ptr<Actor> (a));

  a = new Sphere (VEC3 (-2.0, 2.0, 2.0), 1.0);
  a->setColor (VEC3 (0.0, 1.0, 0.0));
  addObject (shared_ptr<Actor> (a));

  a = new Sphere (VEC3 (2.0, 2.0, -2.0), 1.0);
  a->setColor (VEC3 (1.0, 0.0, 0.0));
  addObject (shared_ptr<Actor> (a));

  sprintf (name, "Frames/distro.ppm");
  makeFrame (string (name));
}

void movie (void)
{
  // Constants
  char name[256];
  char root[] = "test";
  VEC3 eye (-4.0, 2.0, 4.0);
  VEC3 lookAt (0.0, 1.0, 0.0);
  VEC3 u (0.0, 1.0, 0.0);

  int x = 500;
  int y = 500;
  int depthLimit = 10;
  // 1900
  int startFrame = 0;
  int endFrame = 0;
  int FPS = 4;
  int FPSz = 2;
  int s = 1900 + (startFrame * FPS);

  MATRIX4 rotationZom;
  float angle = -45.0 * (M_PI / 180.0);
  rotationZom << cos (angle), 0.0, sin (angle), 0.0,
	          0.0, 1.0, 0.0, 0.0,
		  -1.0 * sin (angle), 0.0, cos (angle), 0.0,
		  0.0, 0.0, 0.0, 1.0;
  VEC3 translateZom (-5.0, 0.0, 5.0);

  loadSkeleton ("wobble.asf", "wobble.amc", FPS, s, 300, MATRIX4::Identity (), VEC3::Zero ());
  loadSkeleton ("zombie.asf", "zombie.amc", FPSz, 0, 300, rotationZom, translateZom);
  trackSkeleton (4.0);

  int i;
  Actor *a;

  for (i = startFrame; i < endFrame + 1; i++) {
    setLookAt (eye, lookAt, u);
    setPerspective (1.0, 65.0, (float) x / (float) y);
    setRes (x, y);
    setDepthLimit (depthLimit);
    // setDepthField (0.05, 3.0, 10);
    clearObjects ();
    clearLights ();

    // Storage
    /* a = new Point (VEC3 (7.0, 2.0, 7.0));
    makeLight (a, VEC3 (1.0, 1.0, 1.0), 1.0);
    a = new Point (VEC3 (-7.0, 2.0, -7.0));
    makeLight (a, VEC3 (1.0, 1.0, 1.0), 1.0);
    a = new Point (VEC3 (7.0, 2.0, -7.0));
    makeLight (a, VEC3 (1.0, 1.0, 1.0), 1.0);
    a = new Point (VEC3 (-7.0, 2.0, 7.0));
    makeLight (a, VEC3 (1.0, 1.0, 1.0), 1.0);*/

    addLights (0);

    // a = new Point (VEC3 (1.75, 2.0, 1.5));
    // makeLight (a, VEC3 (1.0, 1.0, 1.0), 1.0);

    // Origin Marker
    // a = new Sphere (VEC3 (0.0, 0.0, 0.0), 0.5);
    // a->setColor (VEC3 (0.0, 0.0, 1.0));
    // addObject (shared_ptr<Actor> (a));

    addFloor ();
    addWalls ();

    sprintf (name, "./Frames/%s.%04d.ppm", root, i);
    makeFrame (string (name));
    cout << "Rendered Frame " << i << endl;
  }

  exit (0);

  // compileMovie (string (root));
}

void addFloor (void) {
  float r, c, h = 0.0;
  float l = 30.0,
	w = 30.0;

  int row = 1,
      col = 1;
  int i, j;

  CustomActor *s0 = new CustomActor ();

  vector<VEC3> v0, n0;
  vector<VEC3I> t0;

  for (i = 0; i < col + 1; ++i) {
    c = (-1.0 * (w * 0.5)) + ((float) i * (w / (float) col));

    for (j = 0; j < row + 1; ++j) {
      r = (-1.0 * (l * 0.5)) + ((float) j * (l / (float) row));
      v0.push_back (VEC3 (c, h, r));
    }
  }

  n0.reserve (2 * row * col);
  t0.reserve (2 * row * col);

  for (i = 0; i < col; ++i) {
    for (j = 0; j < row; ++j) {

      int topl = (i * (row + 1)) + j;
      int topr = ((i + 1) * (row + 1)) + j;
      int botl = (i * (row + 1)) + (j + 1);
      int botr = ((i + 1) * (row + 1)) + (j + 1);

      VEC3 u0 = v0[topl] - v0[topr];
      VEC3 u1 = v0[botl] - v0[topr];
      VEC3 u2 = v0[botr] - v0[topr];

      t0.push_back (VEC3I (topl, topr, botl));
      n0.push_back (u0.cross (u1).normalized ());

      t0.push_back (VEC3I (topr, botr, botl));
      n0.push_back (u1.cross (u2).normalized ());
    }
  }

  vector<Triangle *> tr0;
  tr0.reserve (t0.size ());

  for (i = 0; i < 2 * row * col; i = i + 1) {
    tr0.push_back (new Triangle (v0[t0[i][0]],
				 v0[t0[i][1]],
				 v0[t0[i][2]],
				 n0[i]));
    if (i % 2 == 0) {
      tr0[i]->setColor (VEC3 (1.0, 0.0, 0.0));
    } else {
      tr0[i]->setColor (VEC3 (0.0, 1.0, 0.0));
    }
  }

  s0->addShapes (tr0);
  // s0->rebuildTree ();

  addObject (shared_ptr<Actor> (s0));
}

void addWalls (void) {
  const int nWalls = 7;
  float r, c, h = 0.0;
  
  float l, w;

  int row, col;
  int i, j, k;

  VEC3 rotA, transl, rotB;

  for (k = 0; k < nWalls; ++k) {
    // Long side wall
    if (k == 0) {
      row = 1;
      col = 1;
      l = 4.0;
      w = 50.0;
    
    // Short room end wall
    } else if (k == 1) {
      row = 1;
      col = 1;
      l = 4.0;
      w = 20.0;

    // Middle walls
    } else if (k > 1 && k < 6) {
      row = 1;
      col = 1;
      l = 4.0;
      w = 20.0;

    // Short room side walls
    } else {
      row = 1;
      col = 1;
      l = 2.0;
      w = 4.0;
    }

    if (k == 0) {
      rotA = VEC3 (90.0, 0.0, 0.0);
      transl = VEC3 (0.0, l * 0.5, -2.0);
      rotB = VEC3 (0.0, 45.0, 0.0);

    } else if (k == 1) {
      rotA = VEC3 (90.0, 0.0, 0.0);
      transl = VEC3 (w * 0.5 - 2.25, l * 0.5, 3.5);
      rotB = VEC3 (0.0, 45.0, 0.0);

    } else if (k == 2) {
      rotA = VEC3 (90.0, 0.0, 0.0);
      transl = VEC3 (w * 0.5 + 0.25, l * 0.5, 1.5);
      rotB = VEC3 (0.0, 45.0, 0.0);

    } else if (k == 3) {
      rotA = VEC3 (90.0, 0.0, 0.0);
      transl = VEC3 (w * 0.5 + 0.25, l * 0.5, 1.4);
      rotB = VEC3 (0.0, 45.0, 0.0);

    } else if (k == 4) {
      rotA = VEC3 (90.0, 0.0, 0.0);
      transl = VEC3 (w * -0.5 - 1.25, l * 0.5, 1.5);
      rotB = VEC3 (0.0, 45.0, 0.0);

    } else if (k == 5) {
      rotA = VEC3 (90.0, 0.0, 0.0);
      transl = VEC3 (w * -0.5 - 1.25, l * 0.5, 1.4);
      rotB = VEC3 (0.0, 45.0, 0.0);

    } else if (k == 6) {
      rotA = VEC3 (0.0, 0.0, 90.0);
      transl = VEC3 (-2.25, w * 0.5, l * 0.5 + 1.5);
      rotB = VEC3 (0.0, 45.0, 0.0);

    } else if (k == 7) {
      rotA = VEC3 (0.0, 0.0, 90.0);
      transl = VEC3 (l * 0.5 - 1.0, w * 0.5, l * 0.5 + 1.5);
      rotB = VEC3 (0.0, 45.0, 0.0);

    }

    vector<VEC3> v, n;
    vector<VEC3I> t;

    for (i = 0; i < col + 1; ++i) {
      c = (-1.0 * (w * 0.5)) + ((float) i * (w / (float) col));

      for (j = 0; j < row + 1; ++j) {
	r = (-1.0 * (l * 0.5)) + ((float) j * (l / (float) row));
	v.push_back (VEC3 (c, h, r));
      }
    }

    n.reserve (2 * row * col);
    t.reserve (2 * row * col);

    for (i = 0; i < col; ++i) {
      for (j = 0; j < row; ++j) {

	int topl = (i * (row + 1)) + j;
	int topr = ((i + 1) * (row + 1)) + j;
	int botl = (i * (row + 1)) + (j + 1);
	int botr = ((i + 1) * (row + 1)) + (j + 1);

	VEC3 u0 = v[topl] - v[topr];
	VEC3 u1 = v[botl] - v[topr];
	VEC3 u2 = v[botr] - v[topr];

	t.push_back (VEC3I (topl, topr, botl));
	n.push_back (u0.cross (u1).normalized ());

	t.push_back (VEC3I (topr, botr, botl));
	n.push_back (u1.cross (u2).normalized ());
      }
    }

    vector<Triangle *> tr;
    tr.reserve (t.size ());

    for (i = 0; i < 2 * row * col; i = i + 1) {

      tr.push_back (new Triangle (v[t[i][0]],
				  v[t[i][1]],
				  v[t[i][2]],
				  n[i]));
      if (i % 2 == 0) {
	tr[i]->setColor (VEC3 (1.0, 0.0, 0.0));
      } else {
	tr[i]->setColor (VEC3 (0.0, 1.0, 0.0));
      }
    }

    CustomActor *s = new CustomActor ();
    s->addShapes (tr);

    s->rotate (rotA);
    s->translate (transl);
    s->rotate (rotB);
    // s->rebuildTree ();

    addObject (shared_ptr<Actor> (s));
  }
}

void addLights (int state) {
  const int nLights = 3;

  int h, i, j, k, subs = 0;
  float r = 0.15;

  VEC3 rotA, transl, rotB;

  for (k = 0; k < nLights; ++k) {
    rotA = VEC3 (0.0, 0.0, 0.0);

    if (k == 0) {
      transl = VEC3 (3.5, 2.0, 1.4 - _epsilon);
    } else if (k == 1) {
      transl = VEC3 (-3.5, 2.0, 1.4 - _epsilon);
    } else if (k == 2) {
      transl = VEC3 (0.0, 2.0, 3.5 - _epsilon);
    }

    rotB = VEC3 (0.0, 45.0, 0.0);

    vector<VEC3> v, n;
    vector<VEC3I> t;

    v.push_back (VEC3 (-1.0 * r, 0.0, 0.0));
    v.push_back (VEC3 (r, 0.0, 0.0));
    v.push_back (VEC3 (0.0, -1.0 * r, 0.0));
    v.push_back (VEC3 (0.0, r, 0.0));
    v.push_back (VEC3 (0.0, 0.0, -1.0 * r));

    t.push_back (VEC3I (1, 3, 4));
    t.push_back (VEC3I (4, 3, 0));
    t.push_back (VEC3I (1, 2, 4));
    t.push_back (VEC3I (4, 2, 0));

    n.reserve (t.size ());
    for (i = 0; i < t.size (); ++i) {
      n[i] = VEC3 (0.0, 0.0, 0.0);

      for (j = 0; j < 3; ++j) {
	n[i] = n[i] + v[t[i][j]];
      }
      n[i] = n[i].normalized ();
    }

    int divs = subs;
    vector<VEC3I> t0;
    while (divs--) {
      for (i = 0; i < t.size (); ++i) {
	h = v.size ();
	
	v.push_back ((v[t[i][0]] + v[t[i][1]]).normalized () * r);
	v.push_back ((v[t[i][1]] + v[t[i][2]]).normalized () * r);
	v.push_back ((v[t[i][2]] + v[t[i][0]]).normalized () * r);

	t0.push_back (VEC3I (t[i][0], h, h + 2));
	t0.push_back (VEC3I (t[i][1], h, h + 1));
	t0.push_back (VEC3I (t[i][2], h + 1, h + 2));
	t0.push_back (VEC3I (h, h + 1, h + 2));
      }
      
      t.clear ();
      swap (t, t0);

      n.reserve (t.size ());
      for (i = 0; i < t.size (); ++i) {
	n[i] = VEC3 (0.0, 0.0, 0.0);

	for (j = 0; j < 3; ++j) {
	  n[i] = n[i] + v[t[i][j]];
	}
	n[i] = n[i].normalized ();
      }
    }

    vector<Triangle *> tr;
    tr.reserve (t.size ());
    tr.clear ();

    for (i = 0; i < t.size (); ++i) {
      tr.push_back (new Triangle (v[t[i][0]], v[t[i][1]], v[t[i][2]], n[i]));
      
      if (i % 2 == 0) {
	tr[i]->setColor (VEC3 (0.25, 0.82, 0.96));
      } else {
	tr[i]->setColor (VEC3 (0.25, 0.96, 0.82));
      }
    }

    CustomActor *s = new CustomActor ();
    s->addShapes (tr);
    s->rotate (rotA);
    s->translate (transl);
    s->rotate (rotB);
    // s->rebuildTree ();

    shared_ptr<Actor> sp (s);
    shared_ptr<Light> l (new Light (sp, VEC3 (0.25, 0.82, 0.96), 0.95));

    addObject (l);
    addLight (l);
  }
}
