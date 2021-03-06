#include "MAIN.h"
#include "mpi.h"
// Start work on OpenMP
using namespace std;

void addFloor (void);
void addWalls (void);
void addLights (int);
void movie (void);
void test (int , int);

int main (int argc, char **argv)
{
  MPI_Init (&argc, &argv); 
  // movie ()
  if (argc == 3) {
    int state = atoi (argv[1]);
    int num = atoi (argv[2]);
    test (state, num);
  }
  MPI_Finalize ();
  return EXIT_SUCCESS;
}

void test (int state, int num)
{
  char name[256];
  VEC3 eye;
  if (state == 0 || state == 1) {
    eye = VEC3 (0.0, 8.0, 8.0);
  }

  if (state == 2 || state == 3) {
    eye = VEC3 (0.0, 2.0, 8.0);
  }
  VEC3 lookAt (0.0, 1.0, 0.0);
  VEC3 u (0.0, 1.0, 0.0);

  int x = 500;
  int y = 500;
  int depthLimit = 10;

  setLookAt (eye, lookAt, u);
  setPerspective (1.0, 65.0, (float) x / (float) y);
  setRes (x, y);
  setDepthLimit (depthLimit);

  if (state == 3) {
    setDepthField (0.5, 8.0, 100);
  }
  
  int rank, size;
  MPI_Comm_size (MPI_COMM_WORLD, &size);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);

  int step = num / size;
  int s = rank * step;
  int e = s + step;


  for (int i = s; i < e; ++i) {
    clearObjects ();
    clearLights (); 

    shared_ptr<Triangle> t0 = make_shared<Triangle> (VEC3 (-10.0, 0.0, -10.0),
                                                     VEC3 (10.0, 0.0, -10.0),
                                                     VEC3 (-10.0, 0.0, 10.0),
                                                     VEC3 (0.0, 1.0, 0.0));
    t0->setColor (VEC3 (0.0, 1.0, 0.0));
    addObject (t0);

    shared_ptr<Triangle> t1 = make_shared<Triangle> (VEC3 (10.0, 0.0, 10.0),
                                                     VEC3 (10.0, 0.0, -10.0),
                                                     VEC3 (-10.0, 0.0, 10.0),
                                                     VEC3 (0.0, 1.0, 0.0));
    t1->setColor (VEC3 (1.0, 0.0, 0.0));
    addObject (t1);

    shared_ptr<Sphere> s0 = make_shared<Sphere> (VEC3 (0.0, 2.0, 0.0), 1.0);
    s0->setColor (VEC3 (0.0, 0.0, 1.0));
    addObject (s0);
    
    
    if (state == 0) {
      shared_ptr<Point> s1 = make_shared<Point> (VEC3 (-2.0, 3.0, -2.0));
      shared_ptr<Light> l0 = make_shared<Light> (s1, VEC3 (1.0, 1.0, 1.0), 1.0);
      addObject (l0);
      addLight (l0);
    } else if (state == 1) {
      shared_ptr<Sphere> s2 = make_shared<Sphere> (VEC3 (-2.0, 3.0, -2.0), 0.25);
      s2->setColor (VEC3 (1.0, 1.0, 1.0));
      shared_ptr<Light> l1 = make_shared<Light> (s2, VEC3 (1.0, 1.0, 1.0), 1.0);
      addObject (l1);
      addLight (l1);
    } else if (state == 2 || state == 3) {
      shared_ptr<Point> s5 = make_shared<Point> (VEC3 (2.0, 3.0, 2.0));
      shared_ptr<Light> l3 = make_shared<Light> (s5, VEC3 (1.0, 1.0, 1.0), 1.0);
      addObject (l3);
      addLight (l3);
    }


    if (state == 2 || state == 3) {
     shared_ptr<Sphere> s3 = make_shared<Sphere> (VEC3 (2.0, 2.0, -2.0), 1.0);
     s3->setColor (VEC3 (1.0, 0.0, 0.0));
     addObject (s3);

     shared_ptr<Sphere> s4 = make_shared<Sphere> (VEC3 (-2.0, 2.0, 2.0), 1.0);
     s4->setColor (VEC3 (0.0, 1.0, 0.0));
     addObject (s4);
    }
    
    sprintf (name, "Tests/424_Test_%02d_%02d_%02d.ppm", state, i, size);
    makeFrame (string (name));
  }
}

void movie (void)
{
  // Constants to produce images
  char name[256];
  char root[] = "final";
  VEC3 eye (8.0, 8.0, -8.0);
  VEC3 lookAt (0.0, 1.0, 0.0);
  VEC3 u (0.0, 1.0, 0.0);

  int x = 1000;
  int y = 1000;
  int depthLimit = 10;
  
  // Starting Frame = 1900
  // Ending Frame = 2750
  // Variables to adjust movie start and end
  int startFrame = 0;
  int endFrame = 300;
  int FPS = 4;
  int FPSz = 1;

  // Require zombie variables
  MATRIX4 rotationZomOne, rotationZomTwo;
  float angleOne = -45.0 * (M_PI / 180.0);
  rotationZomOne << cos (angleOne), 0.0, sin (angleOne), 0.0,
	          0.0, 1.0, 0.0, 0.0,
		  -1.0 * sin (angleOne), 0.0, cos (angleOne), 0.0,
		  0.0, 0.0, 0.0, 1.0;
  VEC3 translateZomOne (-5.0, 0.0, 5.0);

  float angleTwo = 135.0 * (M_PI / 180.0);
  rotationZomTwo << cos (angleTwo), 0.0, sin (angleTwo), 0.0,
	          0.0, 1.0, 0.0, 0.0,
		  -1.0 * sin (angleTwo), 0.0, cos (angleTwo), 0.0,
		  0.0, 0.0, 0.0, 1.0;
  VEC3 translateZomTwo (7.0, 0.0, -3.0);


  // Iterative and MPI variables
  int i, rank, size, rankStart, rankEnd;

  // Event variables
  int eventOne = 0;
  float eventTwo = 2.0;

  // Get MPI values
  MPI_Comm_size (MPI_COMM_WORLD, &size);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);

  if (size > (endFrame - startFrame)) {
    rankStart = startFrame + rank;
    rankEnd = rankStart + 1;

  } else if ((endFrame - startFrame) % size == 0) {
    int step = (endFrame - startFrame) / size;
    rankStart = startFrame + rank * step;
    rankEnd = rankStart + step;
  } else {
    rankStart = 0;
    rankEnd = 0;
  }

  if (rankEnd > rankStart && rankEnd <= endFrame) {
    int s = 1900 + (rankStart * FPS);
    int e = (int) ((2740.0 - (float) s) / (float) FPS);
    int szOne = (rankStart * FPSz) - (65 * FPSz);
    int szTwo = (rankStart * FPSz) - (205 * FPSz);

    szOne = (szOne < 0) ? 0 : szOne;
    szTwo = (szTwo < 0) ? 0 : szTwo;
  
    // Load up the actors
    loadSkeleton ("wobble.asf", "wobble.amc", FPS, s, e, MATRIX4::Identity (), VEC3::Zero ());
    
    for (i = rankStart; i < rankEnd; i++) {

      if (i > 64 && eventOne == 0) {
        loadSkeleton ("zombie.asf", "zombie.amc", FPSz, szOne, 300, rotationZomOne, translateZomOne);
        eventOne = 1;
      }

      if (i > 205 && eventOne == 1) {
        loadSkeleton ("zombie.asf", "zombie.amc", FPSz + 1, szTwo, 300, rotationZomTwo, translateZomTwo);
        eventOne = 2;
      }

      if (i > 220 && eventTwo < 3.75) {
        float POF = 240.0;
        eventTwo = 2.0 + 1.75 * (((float) i - 220.0) / (POF - 220.0));
      }

      setLookAt (eye, lookAt, u);
      trackSkeleton (1.0);
      setPerspective (1.0, 65.0, (float) x / (float) y);
      setRes (x, y);
      setDepthLimit (depthLimit);
      // Add DOF
      setDepthField (0.07, eventTwo, 10);
      clearObjects ();
      clearLights ();
      
      addLights (eventOne);
      addFloor ();
      addWalls ();
      
      sprintf (name, "./Frames/%s.%04d.ppm", root, i);
      makeFrame (string (name));
    }
  }

  MPI_Barrier (MPI_COMM_WORLD);
  if (rank == 0) {
    compileMovie (string (root));
  }
}

void addFloor (void) {
  const int nFloors = 2;
  float r, c, h = 0.0;
  float l = 30.0,
	w = 30.0;

  int row = 1,
      col = 1;
  int i, j, k;

  for (k = 0; k < nFloors; ++k) {

    if (k == 1) h = 4.0;

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
      tr0[i]->setColor (VEC3 (0.689, 0.764, 0.764));
    }
    
    shared_ptr<CustomActor> s0 = make_shared<CustomActor> ();
    s0->addShapes (tr0);
    s0->rebuildTree ();
    addObject (s0);
  }
}

void addWalls (void) {
  const int nWalls = 8;
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

    // Short room side wall
    } else if (k == 6) {
      row = 1;
      col = 1;
      l = 2.0;
      w = 4.0;

    // Mirror
    } else {
      row = 1;
      col = 1;
      l = 1.5;
      w = 2.0;
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
      transl = VEC3 (-2.25 + _epsilon, w * 0.5 + 1.0, 2.5);
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
      if (k == 7) {
        tr[i]->setRefl ();
      } else {
	tr[i]->setColor (VEC3 (0.568, 0.137, 0.137));
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

void addLights (int state) {
  const int nLights = 5;

  int h, i, j, k, subs = 0;
  float r = 0.15;

  float spacing = 10.0 * _epsilon;
  VEC3 rotA, transl, rotB;

  for (k = 0; k < nLights; ++k) {
    if (k == 2 || k == 3) {
      rotA = VEC3 (0.0, 0.0, 0.0);
    } else {
      rotA = VEC3 (-90.0, 0.0, 0.0);
    }

    if (k == 0) {
      transl = VEC3 (2.5, 4.0 - spacing, -0.25);
    } else if (k == 1) {
      transl = VEC3 (-3.5, 4.0 - spacing, -0.25);
    } else if (k == 2) {
      transl = VEC3 (3.25, 3.5, 2.20 - spacing);
    } else if (k == 3) {
      transl = VEC3 (1.75, 3.5, 2.20 - spacing);
    } else if (k == 4) {
      transl = VEC3 (3.0, 4.0 - spacing, 2.5);
    }

    if (k == 2 || k == 3) {
      rotB = VEC3 (0.0, -45.0, 0.0);
    } else {
      rotB = VEC3 (0.0, 45.0, 0.0);
    }

    /*
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
	tr[i]->setColor (VEC3 (0.75, 0.75, 0.75));
      } else {
	tr[i]->setColor (VEC3 (0.75, 0.75, 0.75));
      }
    }

    shared_ptr<CustomActor> s = make_shared<CustomActor> ();
    s->addShapes (tr);
    s->rotate (rotA);
    s->translate (transl);
    s->rotate (rotB);
    s->rebuildTree ();
    */

    shared_ptr<Cylinder> s = make_shared<Cylinder> (VEC3 (0.0, 0.0, 0.0), VEC3 (0.0, 0.0, -0.05), r);
    s->setColor (VEC3 (0.75, 0.75, 0.75));
    s->rotate (rotA);
    s->translate (transl);
    s->rotate (rotB);

    shared_ptr<Light> l (new Light (s, VEC3 (0.75, 0.75, 0.75), 0.95));

    addObject (l);

    if ((k != 1 && k != 4) || (k == 1 && state > 0) || (k == 4 && state > 1)) {
        addLight (l);
    }
  }
}
