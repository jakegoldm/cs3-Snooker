#include "collision.h"
#include "list.h"
#include "shape.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>

const int INF = 10000;

vector_t find_projection(list_t *shape, vector_t axis) {
  double min = DBL_MAX;
  double max = -DBL_MAX;
  for (size_t i = 0; i < list_size(shape); i++) {
    vector_t *vertex = list_get(shape, i);
    double d = vec_dot(axis, *vertex);
    if (d < min) {
      min = d;
    }
    if (d > max) {
      max = d;
    }
  }
  assert(min < max);
  return (vector_t){min, max};
}

double find_overlap(vector_t proj1, vector_t proj2) {
  if (proj1.y < proj2.x || proj2.y < proj1.x) {
    return 0;
  } else if (proj1.x <= proj2.x) {
    return (proj1.y <= proj2.y) ? proj1.y - proj2.x : proj2.y - proj2.x;
  } else {
    return (proj2.y <= proj1.y) ? proj2.y - proj1.x : proj1.y - proj1.x;
  }
}

collision_info_t find_collision(list_t *shape1, list_t *shape2) {
  double min_overlap = DBL_MAX;
  vector_t collision_axis = VEC_ZERO;

  for (size_t i = 0; i < list_size(shape1); i++) {
    vector_t *p1 = list_get(shape1, i);
    vector_t *p2 = list_get(shape1, (i + 1) % list_size(shape1));
    vector_t axis = vec_unit((vector_t){p1->y - p2->y, p2->x - p1->x});
    vector_t proj1 = find_projection(shape1, axis);
    vector_t proj2 = find_projection(shape2, axis);
    double overlap = find_overlap(proj1, proj2);
    if (overlap == 0) {
      return (collision_info_t){false, VEC_ZERO};
    } else if (overlap < min_overlap) {
      min_overlap = overlap;
      collision_axis = axis;
    }
  }

  for (size_t i = 0; i < list_size(shape2); i++) {
    vector_t *p1 = list_get(shape2, i);
    vector_t *p2 = list_get(shape2, (i + 1) % list_size(shape2));
    vector_t axis = vec_unit((vector_t){p1->y - p2->y, p2->x - p1->x});
    vector_t proj2 = find_projection(shape2, axis);
    vector_t proj1 = find_projection(shape1, axis);
    double overlap = find_overlap(proj1, proj2);
    if (overlap == 0) {
      return (collision_info_t){false, VEC_ZERO};
    } else if (overlap < min_overlap) {
      min_overlap = overlap;
      collision_axis = axis;
    }
  }

  return (collision_info_t){true, collision_axis};
}