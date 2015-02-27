/*
 * Random number generator for molecular dynamics simulations
 */
#include <stdlib.h>
#include <math.h>

#ifdef _WINDOWS
#	include <windows.h>
#else
#	include <time.h>
#endif

#include "in_vdefs.h"
#include "simulation.h"

#define IADD   435443298
#define IMUL   314159269
#define MASK   2147483647
#define SCALE  0.4656612873e-9

int randSeedP = 0;

void InitRand (int randSeedI)
{
#ifdef _WINDOWS
	FILETIME        ft;
	ULARGE_INTEGER  uli;
	__int64         t;
#else
	struct timeval tv;
#endif

	// If random seed given, use that
	if (randSeedI != 0) {
		randSeedP = randSeedI;
		message("Random seed = %d (supplied)\n", randSeedP);
		return;
	}

	// Otherwise initialize with time
#ifdef _WINDOWS
	GetSystemTimeAsFileTime(&ft);
	uli.LowPart  = ft.dwLowDateTime;
	uli.HighPart = ft.dwHighDateTime;
	randSeedP  = (int) uli.QuadPart;
#else
	gettimeofday (&tv, 0);
	randSeedP = tv.tv_usec;
#endif

	message("Random seed = %d (time-based)\n", randSeedP);
}

double RandR ()
{
  randSeedP = (randSeedP * IMUL + IADD) & MASK;
  return (randSeedP * SCALE);
}

#if n_dimensions == 2

void VRand (VecR *p)
{
  double s;

  s = 2. * M_PI * RandR ();
  p->x = cos (s);
  p->y = sin (s);
}

#elif n_dimensions == 3

void VRand (VecR *p)
{
  double s, x, y;

  s = 2.;
  while (s > 1.) {
    x = 2. * RandR () - 1.;
    y = 2. * RandR () - 1.;
    s = Sqr (x) + Sqr (y);
  }
  p->z = 1. - 2. * s;
  s = 2. * sqrt (1. - s);
  p->x = s * x;
  p->y = s * y;
}

#else
#	error Number of dimensions must be two or three
#endif

