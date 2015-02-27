/*
 * in_vdefs.h
 *
 * Mathematical macros, including vector operations.
 * 
 * Please see in_mddefs.h for general remarks on Cpp macros.
 */
#ifndef V_DEFS
#define V_DEFS

// The number of dimensions
#define n_dimensions 2

#define M_PI 3.14159265358979323846
// Common math operations
#define Sqr(x)     ((x) * (x))
#define Cube(x)    ((x) * (x) * (x))
#define Sgn(x, y)  (((y) >= 0) ? (x) : (- (x)))
#define IsEven(x)  ((x) & ~1)
#define IsOdd(x)   ((x) & 1)
#define Nint(x)                                             \
   (((x) < 0.) ? (- (int) (0.5 - (x))): ((int) (0.5 + (x))))
#define Min(x1, x2)  (((x1) < (x2)) ? (x1) : (x2))
#define Max(x1, x2)  (((x1) > (x2)) ? (x1) : (x2))
#define Min3(x1, x2, x3) \
   (((x1) < (x2)) ? (((x1) < (x3)) ? (x1) : (x3)) :         \
                    (((x2) < (x3)) ? (x2) : (x3)))
#define Max3(x1, x2, x3) \
   (((x1) > (x2)) ? (((x1) > (x3)) ? (x1) : (x3)) :         \
                    (((x2) > (x3)) ? (x2) : (x3)))
#define Clamp(x, lo, hi)                                    \
   (((x) >= (lo) && (x) <= (hi)) ? (x) :                    \
   (((x) < (lo)) ? (lo) : (hi)))
// Types for 2D and 3D vectors
typedef struct {double x, y;} VecR2;
typedef struct {double x, y, z;} VecR3;
typedef struct {int x, y;} VecI2;
typedef struct {int x, y, z;} VecI3;

// Type for 2D tensors
typedef struct {double xx, xy, yx, yy;} Ten2R2;

#if n_dimensions == 2
/*
 * Vector macros for 2D operations
 *
 * The following macros are vector operations for 2D vectors.
 * Since n_dimensions is set to 2 above, the following are used
 * instead of the 3D versions defined later.
 *
 * Note that some macros return a result, e.g. VDot(), while others
 * only set a variable and have no sensible return value, e.g. VSub().
 * When there is a sensible return value, it is noted with 'Return:'.
 */
typedef VecR2 VecR;
typedef VecI2 VecI;
// Set the components of vector v
#define VSet(v, sx, sy)                                     \
   (v).x = sx,                                              \
   (v).y = sy
   
// Copy vector v2 to v1
#define VCopy(v1, v2)                                       \
   (v1).x = (v2).x,                                         \
   (v1).y = (v2).y
   
// Multiply vector v with scalar s
#define VScale(v, s)                                        \
   (v).x *= s,                                              \
   (v).y *= s
   
// Copy vector v1 to vector v2 and multiply with scalar s
#define VSCopy(v2, s1, v1)                                  \
   (v2).x = (s1) * (v1).x,                                  \
   (v2).y = (s1) * (v1).y
   
// Add vector v2 and v3, result in v1
#define VAdd(v1, v2, v3)                                    \
   (v1).x = (v2).x + (v3).x,                                \
   (v1).y = (v2).y + (v3).y
   
// Substract vector v3 from v2, result in v1
#define VSub(v1, v2, v3)                                    \
   (v1).x = (v2).x - (v3).x,                                \
   (v1).y = (v2).y - (v3).y
   
// Multiply vectors v2 and v3, result in v1
#define VMul(v1, v2, v3)                                    \
   (v1).x = (v2).x * (v3).x,                                \
   (v1).y = (v2).y * (v3).y
   
// Divide vector v2 by v3, result in v1
#define VDiv(v1, v2, v3)                                    \
   (v1).x = (v2).x / (v3).x,                                \
   (v1).y = (v2).y / (v3).y

// Add vector v2 and ( v3 times scalar s3 ), result in v1
#define VSAdd(v1, v2, s3, v3)                               \
   (v1).x = (v2).x + (s3) * (v3).x,                         \
   (v1).y = (v2).y + (s3) * (v3).y
// Add vector v2 and v3, multiplied by scalar s2 and s3 respectively,
// result in v1
#define VSSAdd(v1, s2, v2, s3, v3)                          \
   (v1).x = (s2) * (v2).x + (s3) * (v3).x,                  \
   (v1).y = (s2) * (v2).y + (s3) * (v3).y
// Return: dot product of vectors v1 and v2
#define VDot(v1, v2)                                        \
   ((v1).x * (v2).x + (v1).y * (v2).y)
// Return: scalar triple product of vectors v1, v2 and v3
// see also http://mathworld.wolfram.com/ScalarTripleProduct.html
#define VWDot(v1, v2, v3)                                   \
   ((v1).x * (v2).x * (v3).x + (v1).y * (v2).y * (v3).y)
// Return: perpendicular dot product of vectors v1 and v2
// see also http://mathworld.wolfram.com/PerpDotProduct.html
#define VCross(v1, v2)                                      \
   ((v1).x * (v2).y - (v1).y * (v2).x)

// Return: product of components of vector y
#define VProd(v)                                            \
   ((v).x * (v).y)
// Return: whether both components of v1 are greater than or equal to
//         their v2 counterparts
#define VGe(v1, v2)                                         \
   ((v1).x >= (v2).x && (v1).y >= (v2).y)
// Return: whether both components of v1 are (strictly) smaller than
//         their v2 counterparts
#define VLt(v1, v2)                                         \
   ((v1).x < (v2).x && (v1).y < (v2).y)
// Set all components of vector v to scalar s
#define VSetAll(v, s)                                       \
   VSet (v, s, s)

// Add scalar s to all components of v2, result in v1
#define VAddCon(v1, v2, s)                                  \
   (v1).x = (v2).x + (s),                                   \
   (v1).y = (v2).y + (s)
// Return: vector component; v's x-component if k=0, y-component otherwise
#define VComp(v, k)                                         \
   *((k == 0) ? &(v).x : &(v).y)
// Store vector in array a on n-th index
#define VToLin(a, n, v)                                     \
   a[(n) + 0] = (v).x,                                      \
   a[(n) + 1] = (v).y
// Get vector v from array a on n-th index, result in v
#define VFromLin(v, a, n)                                   \
   VSet (v, a[(n) + 0], a[(n) + 1])
// Return: sum of compontents of vector v
#define VCSum(v)                                            \
   ((v).x + (v).y)

// Increase tensor t by the dyadic product of v with itself
#define TVAddDyad(t, v) \
   TVVAddDyad((t), (v), (v))

// Increase tensor t by the dyadic product of v and w
#define TVVAddDyad(t, v, w) \
   (t).xx += (v).x * (w).x, \
   (t).xy += (v).x * (w).y, \
   (t).yx += (v).y * (w).x, \
   (t).yy += (v).y * (w).y

/*
 * End of 2D vector macros
 */
#endif /* n_dimensions == 2 */

#if n_dimensions == 3
/*
 * Vector macros for 3D operations
 *
 * Please refer to the definitions above, the following is only used when
 * n_dimensions is set to 3, which isn't the case.
 *
 * Please see the 2D versions for comments.
 */
typedef VecR3 VecR;
typedef VecI3 VecI;

#define VSet(v, sx, sy, sz)                                 \
   (v).x = sx,                                              \
   (v).y = sy,                                              \
   (v).z = sz
#define VCopy(v1, v2)                                       \
   (v1).x = (v2).x,                                         \
   (v1).y = (v2).y,                                         \
   (v1).z = (v2).z
#define VScale(v, s)                                        \
   (v).x *= s,                                              \
   (v).y *= s,                                              \
   (v).z *= s
#define VSCopy(v2, s1, v1)                                  \
   (v2).x = (s1) * (v1).x,                                  \
   (v2).y = (s1) * (v1).y,                                  \
   (v2).z = (s1) * (v1).z
#define VAdd(v1, v2, v3)                                    \
   (v1).x = (v2).x + (v3).x,                                \
   (v1).y = (v2).y + (v3).y,                                \
   (v1).z = (v2).z + (v3).z
#define VSub(v1, v2, v3)                                    \
   (v1).x = (v2).x - (v3).x,                                \
   (v1).y = (v2).y - (v3).y,                                \
   (v1).z = (v2).z - (v3).z
#define VMul(v1, v2, v3)                                    \
   (v1).x = (v2).x * (v3).x,                                \
   (v1).y = (v2).y * (v3).y,                                \
   (v1).z = (v2).z * (v3).z
#define VDiv(v1, v2, v3)                                    \
   (v1).x = (v2).x / (v3).x,                                \
   (v1).y = (v2).y / (v3).y,                                \
   (v1).z = (v2).z / (v3).z
#define VSAdd(v1, v2, s3, v3)                               \
   (v1).x = (v2).x + (s3) * (v3).x,                         \
   (v1).y = (v2).y + (s3) * (v3).y,                         \
   (v1).z = (v2).z + (s3) * (v3).z
#define VSSAdd(v1, s2, v2, s3, v3)                          \
   (v1).x = (s2) * (v2).x + (s3) * (v3).x,                  \
   (v1).y = (s2) * (v2).y + (s3) * (v3).y,                  \
   (v1).z = (s2) * (v2).z + (s3) * (v3).z
#define VDot(v1, v2)                                        \
   ((v1).x * (v2).x + (v1).y * (v2).y + (v1).z * (v2).z)
#define VWDot(v1, v2, v3)                                   \
   ((v1).x * (v2).x * (v3).x + (v1).y * (v2).y * (v3).y +   \
   (v1).z * (v2).z * (v3).z)
#define VCross(v1, v2, v3)                                  \
   (v1).x = (v2).y * (v3).z - (v2).z * (v3).y,              \
   (v1).y = (v2).z * (v3).x - (v2).x * (v3).z,              \
   (v1).z = (v2).x * (v3).y - (v2).y * (v3).x
#define MVMul(v1, m, v2)                                         \
   (v1).x = (m)[0] * (v2).x + (m)[3] * (v2).y + (m)[6] * (v2).z, \
   (v1).y = (m)[1] * (v2).x + (m)[4] * (v2).y + (m)[7] * (v2).z, \
   (v1).z = (m)[2] * (v2).x + (m)[5] * (v2).y + (m)[8] * (v2).z
#define MVMulT(v1, m, v2)                                        \
   (v1).x = (m)[0] * (v2).x + (m)[1] * (v2).y + (m)[2] * (v2).z, \
   (v1).y = (m)[3] * (v2).x + (m)[4] * (v2).y + (m)[5] * (v2).z, \
   (v1).z = (m)[6] * (v2).x + (m)[7] * (v2).y + (m)[8] * (v2).z
#define VProd(v)                                            \
   ((v).x * (v).y * (v).z)
#define VGe(v1, v2)                                         \
   ((v1).x >= (v2).x && (v1).y >= (v2).y && (v1).z >= (v2).z)
#define VLt(v1, v2)                                         \
   ((v1).x < (v2).x && (v1).y < (v2).y && (v1).z < (v2).z)
#define VSetAll(v, s)                                       \
   VSet (v, s, s, s)
#define VAddCon(v1, v2, s)                                  \
   (v1).x = (v2).x + (s),                                   \
   (v1).y = (v2).y + (s),                                   \
   (v1).z = (v2).z + (s)
#define VComp(v, k)                                         \
   *((k == 0) ? &(v).x : ((k == 1) ? &(v).y : &(v).z))
#define VToLin(a, n, v)                                     \
   a[(n) + 0] = (v).x,                                      \
   a[(n) + 1] = (v).y,                                      \
   a[(n) + 2] = (v).z
#define VFromLin(v, a, n)                                   \
   VSet (v, a[(n) + 0], a[(n) + 1], a[(n) + 2])
#define VCSum(v)                                            \
   ((v).x + (v).y + (v).z)
   
/*
 * End of 3D vector macros
 */
#endif /* n_dimensions == 3 */
/*
 * Other vector operations that are independent of the number of
 * dimensions used.
 */

// Set all components of vector v to zero
#define VZero(v)  VSetAll (v, 0)

// Return: squared length of vector v
#define VLenSq(v)  VDot (v, v)

// Return: dot product of vector v1 and (vector v2 squared)
#define VWLenSq(v1, v2)  VWDot(v1, v2, v2)

// Return: length of vector v
#define VLen(v)  sqrt (VDot (v, v))

// Add vector v2 to v1, result in v1
#define VVAdd(v1, v2)  VAdd (v1, v1, v2)

// Substract vector v2 from v1, result in v1
#define VVSub(v1, v2)  VSub (v1, v1, v2)

// Add vector v2 times scalar s2 to vector v1, result in v1
#define VVSAdd(v1, s2, v2) VSAdd (v1, v1, s2, v2)
// Get vector interpolated between v2 and v3, place selected by s2 in [0,1],
// result in v1
#define VInterp(v1, s2, v2, v3)                             \
   VSSAdd (v1, s2, v2, 1. - (s2), v3)


// Set all components of tensor t to zero
#define TZero(t) \
   (t).xx = 0.0, \
   (t).xy = 0.0, \
   (t).yx = 0.0, \
   (t).yy = 0.0

#endif /* V_DEFS */
