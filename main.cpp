#include "MAIN.h"

using namespace std;

void benchmark (void);

int main (int argc, char **argv)
{
  // Constants
  char name[256];
  char root[] = "test";
  VEC3 eye (0.0, 8.0, 15.5);
  VEC3 lookAt (0.0, 0.0, 0.0);
  VEC3 u (0.0, 1.0, 0.0);

  int x = 500;
  int y = 500;
  int depthLimit = 10;
  // loadSkeleton ("wobble.asf", "wobble.amc", 5, 1900, 300);

  int i;
  Actor *a;

  for (i = 0; i < 1; i++) {
    setLookAt (eye, lookAt, u);
    setPerspective (1.0, 65.0, (float) x / (float) y);
    setRes (x, y);
    setDepthLimit (depthLimit);
    clearObjects ();
    clearLights ();

    // Storage
    a = new Point (VEC3 (5.0, 1.0, 8.0));
    makeLight (a, VEC3 (1.0, 1.0, 1.0), 1.0);
    a = new Point (VEC3 (-5.0, 1.0, 8.0));
    makeLight (a, VEC3 (1.0, 1.0, 1.0), 1.0);

    a = new Cylinder (VEC3 (0.0, 1.0, 0.0), VEC3 (0.0, 3.0, 0.0), 2.0);
    a->setColor (VEC3 (0.0, 0.0, 1.0));
    addObject (shared_ptr<Actor> (a)); 
    
    makeSquare (10.0, 10.0, 1, 1, VEC3 (0.0, 0.0, 0.0), VEC3 (0.0, 0.0, 0.0)); 

    sprintf (name, "./Frames/%s.%04d.ppm", root, i);
    makeFrame (string (name));
    cout << "Rendered Frame " << i << endl;
  }

  // compileMovie (string (root));

  return EXIT_SUCCESS;
}
