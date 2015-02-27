/*
 * in_mddefs.h
 *
 * Common molecular dynamics macros.
 *
 * If you're not intimately familiar with Cpp macros you might want to keep
 * some references handy in case you encounter problems. There are some
 * subtle issues in using macros. If you don't run into problems, I wouldn't
 * bother too much though.
 * - http://en.wikipedia.org/wiki/C_preprocessor
 * - http://gcc.gnu.org/onlinedocs/cpp/Macros.html  
 *   and especially: http://gcc.gnu.org/onlinedocs/cpp/Macro-Pitfalls.html
 * - http://msdn2.microsoft.com/en-us/library/503x3e3s(VS.80).aspx
 */
#ifndef IN_MDDEFS_H
#define IN_MDDEFS_H

#define AllocMem(a, n, t)  a = (t *) malloc ((n) * sizeof (t))

#define AllocMem2(a, n1, n2, t)                             \
   AllocMem (a, n1, t *);                                   \
   AllocMem (a[0], (n1) * (n2), t);                         \
   for (k = 1; k < n1; k ++) a[k] = a[k - 1] + n2;

// Math and vector macros so we can use them below
#include "in_vdefs.h"

// Do statement following DO_MOL for every molecule, counter is n
#define DO_MOL  for (n = 0; n < nMol; n ++)

// Wrap component t of vector v to region for periodic boundary
// caution: v may not be more than two regions from zero
#define VWrap(v, t)                                         \
   if (v.t >= 0.5 * region.t)      v.t -= region.t;         \
   else if (v.t < -0.5 * region.t) v.t += region.t

#if n_dimensions == 2
/*
 * 2D macros
 *
 * The following is used for current simulation, since in_vdefs.h sets
 * n_dimensions to 2.
 */
 // Wrap all components of vector v to periodic boundary
#define VWrapAll(v)                                         \
   {VWrap (v, x);                                           \
   VWrap (v, y);}

/* End of 2D macros */
#endif /* n_dimensions == 2 */

#if n_dimensions == 3
/*
 * 3D macros
 *
 * Not used for current simulation, since n_dimensions is set to 2.
 */
#define VWrapAll(v)                                         \
   {VWrap (v, x);                                           \
   VWrap (v, y);                                            \
   VWrap (v, z);}

/* End of 3D macros */
#endif /* n_dimensions == 3 */

/*
 * Statistical properties
 */
typedef struct {
  double val, sum, sum2;
} Prop;

// Set property v to zero
#define PropZero(v)  v.sum = v.sum2 = 0.

// Accumulate property, use after you have set v.val
#define PropAccum(v)  v.sum += v.val, v.sum2 += Sqr (v.val)

// Compute the average of property v
// caution: n must match the number of times you called PropAccum()
#define PropAvg(v, n) \
   v.sum /= n, v.sum2 = sqrt (Max (v.sum2 / n - Sqr (v.sum), 0.))

// Return: sum and stdev
// this is used in printf() statements, make sure to have %f twice in the format string
#define PropEst(v)  v.sum, v.sum2

#endif /* IN_MDDEFS_H */
