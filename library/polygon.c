#include "polygon.h"
#include "list.h"
#include <math.h>
#include <stdlib.h>

double polygon_area(list_t *polygon) {
  int len = list_size(polygon);
  double sum = 0;
  for (int i = 0; i < len; i++) {
    sum += (((vector_t *)list_get(polygon, (i + 1) % len))->y +
            ((vector_t *)list_get(polygon, i))->y) *
           (((vector_t *)list_get(polygon, i))->x -
            ((vector_t *)list_get(polygon, (i + 1) % len))->x);
  }
  return sum / 2;
}

vector_t polygon_centroid(list_t *polygon) {
  int len = list_size(polygon);
  vector_t centroid = VEC_ZERO;
  double area = polygon_area(polygon);
  for (int i = 0; i < len; i++) {
    double x_i = ((vector_t *)list_get(polygon, i))->x;
    double x_i1 = ((vector_t *)list_get(polygon, (i + 1) % len))->x;
    double y_i = ((vector_t *)list_get(polygon, i))->y;
    double y_i1 = ((vector_t *)list_get(polygon, (i + 1) % len))->y;

    centroid.x += (x_i + x_i1) * (x_i * y_i1 - x_i1 * y_i);
    centroid.y += (y_i + y_i1) * (x_i * y_i1 - x_i1 * y_i);
  }
  centroid.x /= (6 * area);
  centroid.y /= (6 * area);
  return centroid;
}

vector_t polygon_center(list_t *polygon) {
  int len = list_size(polygon);
  vector_t centroid = VEC_ZERO;
  for (int i = 0; i < len; i++) {
    centroid.x += ((vector_t *)list_get(polygon, i))->x;
    centroid.y += ((vector_t *)list_get(polygon, i))->y;
  }
  centroid.x /= len;
  centroid.y /= len;
  return centroid;
}

void polygon_translate(list_t *polygon, vector_t translation) {
  for (size_t i = 0; i < list_size(polygon); i++) {
    *(vector_t *)list_get(polygon, i) =
        vec_add(*(vector_t *)list_get(polygon, i), translation);
  }
}

void polygon_rotate(list_t *polygon, double angle, vector_t point) {
  for (size_t i = 0; i < list_size(polygon); i++) {
    *(vector_t *)list_get(polygon, i) = vec_add(
        vec_rotate(vec_subtract(*(vector_t *)list_get(polygon, i), point),
                   angle),
        point);
  }
}

void polygon_stretch_x(list_t *polygon, double factor) {
  for (size_t i = 0; i < list_size(polygon); i++) {
    *(vector_t *)list_get(polygon, i) =
        vec_multiply_x(factor, *(vector_t *)list_get(polygon, i));
  }
}

void polygon_reflect_x(list_t *polygon, double x0) {
  for (size_t i = 0; i < list_size(polygon); i++) {
    vector_t *p = list_get(polygon, i);
    p->x = 2 * x0 - p->x;
  }
}

void polygon_reflect_y(list_t *polygon, double y0) {
  for (size_t i = 0; i < list_size(polygon); i++) {
    vector_t *p = list_get(polygon, i);
    p->y = 2 * y0 - p->y;
  }
}

list_t *polygon_copy(list_t *polygon) {
  size_t size = list_size(polygon);
  list_t *new_points = list_init(size, (free_func_t)free);
  for (size_t i = 0; i < size; i++) {
    vector_t *point = malloc(sizeof(vector_t));
    *point = *(vector_t *)list_get(polygon, i);
    list_add(new_points, point);
  }
  return new_points;
}