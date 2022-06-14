#include "vector.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

const vector_t VEC_ZERO = {0, 0};

vector_t vec_add(vector_t v1, vector_t v2) {
  return (vector_t){v1.x + v2.x, v1.y + v2.y};
}

vector_t vec_subtract(vector_t v1, vector_t v2) {
  return (vector_t){v1.x - v2.x, v1.y - v2.y};
}

vector_t vec_negate(vector_t v) { return (vector_t){-1 * v.x, -1 * v.y}; }

vector_t vec_multiply(double scalar, vector_t v) {
  return (vector_t){scalar * v.x, scalar * v.y};
}

vector_t vec_multiply_x(double scalar, vector_t v) {
  return (vector_t){scalar * v.x, v.y};
}

double vec_dot(vector_t v1, vector_t v2) { return v1.x * v2.x + v1.y * v2.y; }

double vec_cross(vector_t v1, vector_t v2) { return v1.x * v2.y - v1.y * v2.x; }

vector_t vec_rotate(vector_t v, double angle) {
  return (vector_t){v.x * cos(angle) - v.y * sin(angle),
                    v.x * sin(angle) + v.y * cos(angle)};
}

double vec_magnitude(vector_t v) { return sqrt(vec_dot(v, v)); }

double vec_direction(vector_t v) {
  if (v.x == 0) {
    return ((v.y > 0) - (v.y < 0)) * M_PI / 2;
  }
  double alpha = atan(v.y / v.x);
  if (v.x > 0) {
    return alpha;
  } else {
    return v.y < 0 ? alpha - M_PI : alpha + M_PI;
  }
}

vector_t vec_init(double m, double d) {
  return (vector_t){m * cos(d), m * sin(d)};
}

vector_t vec_unit(vector_t v) {
  double m = vec_magnitude(v);
  if (m == 0) {
    return VEC_ZERO;
  }
  return (vector_t){v.x / m, v.y / m};
}

bool is_within(double epsilon, double d1, double d2) {
  return fabs(d1 - d2) < epsilon;
}

bool is_close(double d1, double d2) { return is_within(1e-7, d1, d2); }

bool vec_is_within(double epsilon, vector_t v1, vector_t v2) {
  return is_within(epsilon, v1.x, v2.x) && is_within(epsilon, v1.y, v2.y);
}

bool vec_is_equal(vector_t v1, vector_t v2) {
  return v1.x == v2.x && v1.y == v2.y;
}

bool vec_is_close(vector_t v1, vector_t v2) {
  return is_close(v1.x, v2.x) && is_close(v1.y, v2.y);
}

void vec_print(vector_t v) { printf("(%f, %f)\n", v.x, v.y); }

void vector_multiply_2(double magnitude, vector_t vector) {
  vector.x *= magnitude;
  vector.y *= magnitude;
}