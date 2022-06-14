#include "shape.h"
#include <assert.h>
#include <math.h>

const int CIRCLE_POINTS = 60;

double normalize(double angle) { return fmod(angle + 2 * M_PI, 2 * M_PI); }

list_t *draw_helper(size_t n, vector_t *centroid, double inner, double major,
                    double minor, double angle1, double angle2) {
  assert(n >= 3);
  list_t *shape = list_init(n + 2, (free_func_t)free);
  double theta = (angle2 - angle1) / n;
  if (n == CIRCLE_POINTS) {
    n--;
  }
  for (int i = 0; i <= n; i++) {
    double f = i % 2 == 0 ? 1 : inner / major;
    vector_t *p = malloc(sizeof(vector_t));
    *p = (vector_t){centroid->x + f * major * cos(i * theta + angle1),
                    centroid->y + f * minor * sin(i * theta + angle1)};
    list_add(shape, p);
  }
  return shape;
}

list_t *draw_star(size_t n, vector_t *centroid, double inner, double outer) {
  assert(n >= 2);
  return draw_helper(n * 2, centroid, inner, outer, outer, 0, 2 * M_PI);
}

list_t *draw_arc(vector_t *centroid, double radius, double angle1,
                 double angle2) {
  return draw_arc_offset(centroid, radius, angle1, angle2, 0, 0);
}

list_t *draw_arc_offset(vector_t *centroid, double radius, double angle1,
                        double angle2, double x_offset, double y_offset) {
  angle1 = normalize(angle1);
  angle2 = normalize(angle2);
  int num_vertices = CIRCLE_POINTS * (1 - (angle2 - angle1) / (2 * M_PI));
  list_t *points = draw_helper(num_vertices, centroid, radius, radius, radius,
                               angle1, angle2);
  *centroid = (vector_t){centroid->x + x_offset, centroid->y + y_offset};
  list_add(points, centroid);
  return points;
}

list_t *draw_circle(vector_t *centroid, double radius) {
  return draw_ellipse(centroid, radius, radius);
}

list_t *draw_ellipse(vector_t *centroid, double major, double minor) {
  return draw_helper(CIRCLE_POINTS, centroid, major, major, minor, 0, 2 * M_PI);
}

list_t *draw_rectangle(vector_t *centroid, double x, double y) {
  vector_t p1 = {centroid->x - x / 2, centroid->y - y / 2};
  vector_t p2 = {centroid->x + x / 2, centroid->y - y / 2};
  vector_t p3 = {centroid->x + x / 2, centroid->y + y / 2};
  vector_t p4 = {centroid->x - x / 2, centroid->y + y / 2};
  return draw_quadrilateral(p1, p2, p3, p4);
}

list_t *draw_cup(vector_t *edge1, vector_t *edge2, vector_t *centroid,
                 double radius, double angle1, double angle2) {
  int num_vertices = CIRCLE_POINTS * (1 - (angle2 - angle1) / (2 * M_PI));
  list_t *points = draw_helper(num_vertices, centroid, radius, radius, radius,
                               angle1, angle2);
  list_add(points, edge1);
  list_add(points, edge2);
  return points;
}

list_t *draw_quadrilateral(vector_t p_1, vector_t p_2, vector_t p_3,
                           vector_t p_4) {
  list_t *shape = list_init(4, (free_func_t)free);
  vector_t *p1 = malloc(sizeof(vector_t));
  vector_t *p2 = malloc(sizeof(vector_t));
  vector_t *p3 = malloc(sizeof(vector_t));
  vector_t *p4 = malloc(sizeof(vector_t));
  *p1 = p_1;
  *p2 = p_2;
  *p3 = p_3;
  *p4 = p_4;
  list_add(shape, p1);
  list_add(shape, p2);
  list_add(shape, p3);
  list_add(shape, p4);
  return shape;
}

list_t *draw_triangle(vector_t p_1, vector_t p_2, vector_t p_3) {
  list_t *shape = list_init(3, (free_func_t)free);
  vector_t *p1 = malloc(sizeof(vector_t));
  vector_t *p2 = malloc(sizeof(vector_t));
  vector_t *p3 = malloc(sizeof(vector_t));
  *p1 = p_1;
  *p2 = p_2;
  *p3 = p_3;
  list_add(shape, p1);
  list_add(shape, p2);
  list_add(shape, p3);
  return shape;
}