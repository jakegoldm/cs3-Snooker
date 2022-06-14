#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdbool.h>

/**
 * A real-valued 2-dimensional vector.
 * Positive x is towards the right; positive y is towards the top.
 * vector_t is defined here instead of vector.c because it is passed *by value*.
 */
typedef struct {
  double x;
  double y;
} vector_t;

/**
 * The zero vector, i.e. (0, 0).
 * "extern" declares this global variable without allocating memory for it.
 * You will need to define "const vector_t VEC_ZERO = ..." in vector.c.
 */
extern const vector_t VEC_ZERO;

/**
 * Adds two vectors.
 * Performs the usual componentwise vector sum.
 *
 * @param v1 the first vector
 * @param v2 the second vector
 * @return v1 + v2
 */
vector_t vec_add(vector_t v1, vector_t v2);

/**
 * Subtracts two vectors.
 * Performs the usual componentwise vector difference.
 *
 * @param v1 the first vector
 * @param v2 the second vector
 * @return v1 - v2
 */
vector_t vec_subtract(vector_t v1, vector_t v2);

/**
 * Computes the additive inverse a vector.
 * This is equivalent to multiplying by -1.
 *
 * @param v the vector whose inverse to compute
 * @return -v
 */
vector_t vec_negate(vector_t v);

/**
 * Multiplies a vector by a scalar.
 * Performs the usual componentwise product.
 *
 * @param scalar the number to multiply the vector by
 * @param v the vector to scale
 * @return scalar * v
 */
vector_t vec_multiply(double scalar, vector_t v);

/**
 * Multiplies a vector by a scalar in the x direction.
 * Performs the usual componentwise product.
 *
 * @param scalar the number to multiply the vector in the x direction by
 * @param v the vector to scale
 * @return scalar * v.x
 */
vector_t vec_multiply_x(double scalar, vector_t v);

/**
 * Computes the dot product of two vectors.
 * See https://en.wikipedia.org/wiki/Dot_product#Algebraic_definition.
 *
 * @param v1 the first vector
 * @param v2 the second vector
 * @return v1 . v2
 */
double vec_dot(vector_t v1, vector_t v2);

/**
 * Computes the cross product of two vectors,
 * which lies along the z-axis.
 * See https://en.wikipedia.org/wiki/Cross_product#Computing_the_cross_product.
 *
 * @param v1 the first vector
 * @param v2 the second vector
 * @return the z-component of v1 x v2
 */
double vec_cross(vector_t v1, vector_t v2);

/**
 * Rotates a vector by an angle around (0, 0).
 * The angle is given in radians.
 * Positive angles are counterclockwise, according to the right hand rule.
 * See https://en.wikipedia.org/wiki/Rotation_matrix.
 * (You can derive this matrix by noticing that rotation by a fixed angle
 * is linear and then computing what it does to (1, 0) and (0, 1).)
 *
 * @param v the vector to rotate
 * @param angle the angle to rotate the vector
 * @return v rotated by the given angle
 */
vector_t vec_rotate(vector_t v, double angle);

/**
 * @brief Returns the magnitude of a given vector
 *
 * @param v the vector to get the magnitude of
 * @return the magnitude/norm of the given vector
 */
double vec_magnitude(vector_t v);

/**
 * @brief Return the direction of the vector from 0 to 2pi
 *
 * @param v the vector to get the direction of
 * @return the direction of the vector from 0 to 2pi radians
 */
double vec_direction(vector_t v);

/**
 * @brief Return a vector given a magnitude and direction
 *
 * @param m desired magnitude
 * @param d desired direction
 * @return vector with specified magnitude and direction
 */
vector_t vec_init(double m, double d);

/**
 * @brief Return a unit vector in the direction of a given vector
 *
 * @param v the input vector
 * @return the unit vector
 */
vector_t vec_unit(vector_t v);

/**
 * Return if the components of two vectors are exactly equal.
 * Floating-point math is approximate, so the result of a
 * series of vector computations might not be exactly as expected.
 */
bool vec_is_equal(vector_t v1, vector_t v2);

/**
 * Return if two vectors are close to each other; that is, if the corresponding
 * components are within 1e-7 of each other.
 * This may be more useful than vec_equal, because vector components are
 * doubles, not integers, and floating-point math is approximate.
 */
bool vec_is_close(vector_t v1, vector_t v2);

/**
 * Returns whether two vectors are nearly equal,
 * where the acceptable difference of each component is specified by epsilon.
 */
bool vec_is_within(double epsilon, vector_t v1, vector_t v2);

void vec_print(vector_t v);
/**
 * Void version of vec_multiply
 * @param magnitude the magnitude to multiply the vector by
 * @param v the vector to multiply
 */
void vector_multiply_2(double magnitude, vector_t v);

#endif // #ifndef __VECTOR_H__
