#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "color.h"
#include "list.h"
#include "vector.h"
#include <stddef.h>
#include <stdlib.h>

list_t *draw_star(size_t n, vector_t *centroid, double inner, double outer);

list_t *draw_arc(vector_t *centroid, double radius, double angle1,
                 double angle2);

list_t *draw_arc_offset(vector_t *centroid, double radius, double angle1,
                        double angle2, double x_offset, double y_offset);

list_t *draw_circle(vector_t *centroid, double radius);

list_t *draw_ellipse(vector_t *centroid, double major, double minor);

list_t *draw_rectangle(vector_t *centroid, double x, double y);

list_t *draw_cup(vector_t *edge1, vector_t *edge2, vector_t *centroid,
                 double radius, double angle1, double angle2);

list_t *draw_quadrilateral(vector_t p1, vector_t p2, vector_t p3, vector_t p4);

list_t *draw_triangle(vector_t p_1, vector_t p_2, vector_t p_3);

#endif
