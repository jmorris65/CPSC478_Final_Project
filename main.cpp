#include "MAIN.h"

using namespace std;

void benchmark (void);

int main (int argc, char **argv)
{
  // Constants
  char name[256];
  char root[] = "test";
  VEC3 eye (3.0, 8.0, 3.0);
  VEC3 lookAt (0.0, 1.0, 0.0);
  VEC3 u (0.0, 1.0, 0.0);

  int x = 500;
  int y = 500;
  int depthLimit = 10;
  // 1850
  loadSkeleton ("wobble.asf", "wobble.amc", 5, 2275, 300);

  int i;
  Actor *a;
  float height, width, dist, zx, yt, stride;

  for (i = 0; i < 1; i++) {
    setLookAt (eye, lookAt, u);
    setPerspective (1.0, 65.0, (float) x / (float) y);
    setRes (x, y);
    setDepthLimit (depthLimit);
    clearObjects ();
    clearLights ();

    // Storage
    a = new Point (VEC3 (5.0, 1.0, 6.0));
    makeLight (a, VEC3 (1.0, 1.0, 1.0), 1.0);
    a = new Point (VEC3 (-5.0, 1.0, 6.0));
    makeLight (a, VEC3 (1.0, 1.0, 1.0), 1.0);
    a = new Point (VEC3 (5.0, 1.0, -6.0));
    makeLight (a, VEC3 (1.0, 1.0, 1.0), 1.0);
    a = new Point (VEC3 (-5.0, 1.0, -6.0));
    makeLight (a, VEC3 (1.0, 1.0, 1.0), 1.0);

    height = 3.0;
    width = 5.5;
    dist = (10.0 - width) / 2.0;
    zx = sqrt ((dist * dist) / 2.0);
    yt = height / 2.0;
    stride = 0.2;
    makeSquare (height, width, 1, 1, VEC3 (zx + stride, yt, -1.0 * zx + stride), VEC3 (90.0, 45.0, 0.0));
    
    width = 3.5;
    dist = (10.0 - width) / 2.0;
    zx = sqrt ((dist * dist) / 2.0);
    stride = 0.2;
    makeSquare (height, width, 1, 1, VEC3 (-1.0 * zx + stride, yt, zx + stride), VEC3 (90.0, 45.0, 0.0));

    height = 1.0;
    width = 1.0;
    dist = 1.0;
    zx = sqrt ((dist * dist) / 2.0);
    yt = (height / 2.0) + 2.0;
    stride = 0.2;
    makeSquare (height, width, 1, 1, VEC3 (-1.0 * zx + stride, yt, zx + stride), VEC3 (90.0, 45.0, 0.0));

    makeSquare (10.0, 10.0, 1, 1, VEC3 (0.0, 0.0, 0.0), VEC3 (0.0, 45.0, 0.0)); 

    sprintf (name, "./Frames/%s.%04d.ppm", root, i);
    makeFrame (string (name));
    cout << "Rendered Frame " << i << endl;
  }

  // compileMovie (string (root));

  return EXIT_SUCCESS;
}
