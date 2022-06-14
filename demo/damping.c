#include "body.h"
#include "forces.h"
#include "list.h"
#include "polygon.h"
#include "sdl_wrapper.h"
#include "shape.h"
#include "state.h"
#include "vector.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

const vector_t MIN_POS = {0, 0};
const vector_t MAX_POS = {2000, 1000};
const int POLYGON_COUNT = 48;
const double K = 5000;
const double GAMMA = 0.5;
const double INIT_SPEED = 75;

typedef struct state {
  scene_t *scene;
  rgb_color_t *rainbow;
  size_t curr_color;
} state_t;

void generate_new_shape(state_t *state, int i, double radius) {
  double mass = radius * radius;

  vector_t *centroid = malloc(sizeof(vector_t));
  vector_t *eq_pos = malloc(sizeof(vector_t));
  *eq_pos = (vector_t){radius * (2 * i + 1), (MAX_POS.y - MIN_POS.y) / 2};
  *centroid = vec_add(*eq_pos, (vector_t){0, 0 * MAX_POS.y / 10});

  list_t *shape = draw_circle(centroid, radius);
  rgb_color_t color = state->rainbow[state->curr_color];
  state->curr_color = (state->curr_color + 1) % (POLYGON_COUNT);
  body_t *body = body_init(shape, mass, color);
  vector_t v = {0, INIT_SPEED * sqrt(POLYGON_COUNT - i) *
                       sin(M_PI * i / (POLYGON_COUNT / 3))};
  body_set_velocity(body, v);
  scene_add_body(state->scene, body);
}

void apply_forces(state_t *state) {
  size_t body_count = scene_bodies(state->scene);
  for (size_t i = 0; i < body_count - 1; i++) {
    body_t *body1 = scene_get_body(state->scene, i);
    body_t *body2 = scene_get_body(state->scene, i + 1);
    create_spring(state->scene, K, body1, body2);
    create_drag(state->scene, GAMMA, body1);
  }
  create_drag(state->scene, GAMMA,
              scene_get_body(state->scene, scene_bodies(state->scene) - 1));
}

void generate_spring(state_t *state) {
  double radius = (MAX_POS.x - MIN_POS.x) / (2 * POLYGON_COUNT);

  vector_t *centroid = malloc(sizeof(vector_t));
  *centroid = (vector_t){MIN_POS.x - radius, MAX_POS.y / 2};
  list_t *points_l = draw_circle(centroid, radius);
  *centroid = (vector_t){MAX_POS.x + radius, MAX_POS.y / 2};
  list_t *points_r = draw_circle(centroid, radius);
  body_t *anchor_l = body_init(points_l, DBL_MAX, (rgb_color_t){0, 0, 0});
  body_t *anchor_r = body_init(points_r, DBL_MAX, (rgb_color_t){0, 0, 0});

  scene_add_body(state->scene, anchor_l);
  for (int i = 0; i < POLYGON_COUNT; i++) {
    generate_new_shape(state, i, radius);
  }
  scene_add_body(state->scene, anchor_r);
  apply_forces(state);
}

state_t *emscripten_init() {
  state_t *state = malloc(sizeof(state_t));
  assert(state != NULL);
  sdl_init(MIN_POS, MAX_POS);
  state->scene = scene_init();
  state->rainbow = create_rainbow(POLYGON_COUNT / 6);
  state->curr_color = 0;
  generate_spring(state);
  return state;
}

void emscripten_main(state_t *state) {
  sdl_render_scene(state->scene);
  scene_tick(state->scene, time_since_last_tick());
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state->rainbow);
  free(state);
}
