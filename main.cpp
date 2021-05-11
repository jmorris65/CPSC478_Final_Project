#include "MAIN.h"

using namespace std;

void addFloor (void);
void addWalls (void);

int main (int argc, char **argv)
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
  int endFrame = 300;
  int FPS = 4;
  int s = 1900 + (startFrame * FPS);

  loadSkeleton ("wobble.asf", "wobble.amc", FPS, s, 300);
  trackSkeleton ();

  int i;
  Actor *a;

  for (i = startFrame; i < endFrame + 1; i++) {
    setLookAt (eye, lookAt, u);
    setPerspective (1.0, 65.0, (float) x / (float) y);
    setRes (x, y);
    setDepthLimit (depthLimit);
    clearObjects ();
    clearLights ();

    // Storage
    a = new Point (VEC3 (7.0, 2.0, 7.0));
    makeLight (a, VEC3 (1.0, 1.0, 1.0), 1.0);
    a = new Point (VEC3 (-7.0, 2.0, -7.0));
    makeLight (a, VEC3 (1.0, 1.0, 1.0), 1.0);
    a = new Point (VEC3 (7.0, 2.0, -7.0));
    makeLight (a, VEC3 (1.0, 1.0, 1.0), 1.0);
    a = new Point (VEC3 (-7.0, 2.0, 7.0));
    makeLight (a, VEC3 (1.0, 1.0, 1.0), 1.0);

    a = new Point (VEC3 (1.75, 2.0, 1.5));
    makeLight (a, VEC3 (1.0, 1.0, 1.0), 1.0);

    // Origin Marker
    // a = new Sphere (VEC3 (0.0, 0.0, 0.0), 1.0);
    // a->setColor (VEC3 (0.0, 0.0, 1.0));
    // addObject (shared_ptr<Actor> (a));

    addFloor ();
    addWalls ();

    sprintf (name, "./Frames/%s.%04d.ppm", root, i);
    makeFrame (string (name));
    cout << "Rendered Frame " << i << endl;
  }

  compileMovie (string (root));

  return EXIT_SUCCESS;
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
  s0->rebuildTree ();

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
    s->rebuildTree ();

    addObject (shared_ptr<Actor> (s));
  }
}
