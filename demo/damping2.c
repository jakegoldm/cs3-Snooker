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
const double K = 3000;
const double GAMMA = 0.2;
const double INIT_SPEED = 75;

typedef struct state {
  scene_t *scene;
  rgb_color_t *rainbow;
  size_t curr_color;
} state_t;

void generate_new_shape(state_t *state, int i) {
  double radius = (MAX_POS.x - MIN_POS.x) / (2 * POLYGON_COUNT);
  double mass = radius * radius;

  vector_t *centroid = malloc(sizeof(vector_t));
  vector_t *eq_pos = malloc(sizeof(vector_t));
  *eq_pos = (vector_t){radius * (2 * i + 1), (MAX_POS.y - MIN_POS.y) / 2};
  double y_pos =
      (1 - 2 / (1 + exp(-(i - POLYGON_COUNT * 1 / 2) / (sqrt(POLYGON_COUNT)))));
  *centroid = vec_add(*eq_pos, (vector_t){0, MAX_POS.y / 2 * y_pos});

  list_t *anchor_points = draw_circle(eq_pos, radius);
  body_t *anchor = body_init(anchor_points, INFINITY, WHITE);
  scene_add_body(state->scene, anchor);

  list_t *shape = draw_circle(centroid, radius);
  rgb_color_t color = state->rainbow[state->curr_color];
  state->curr_color = (state->curr_color + 1) % (POLYGON_COUNT);
  body_t *body = body_init(shape, mass, color);
  scene_add_body(state->scene, body);

  double gamma = GAMMA / (fabs(pow(y_pos, 2)) + 1e-4);
  create_spring(state->scene, K, anchor, body);
  create_drag(state->scene, gamma, body);
}

void generate_spring(state_t *state) {
  for (int i = 0; i < POLYGON_COUNT; i++) {
    generate_new_shape(state, i);
  }
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
