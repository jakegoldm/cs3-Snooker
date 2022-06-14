#include "body.h"
#include "color.h"
#include "forces.h"
#include "list.h"
#include "polygon.h"
#include "sdl_wrapper.h"
#include "shape.h"
#include "state.h"
#include "vector.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

const vector_t MIN_POS = {0, 0};
const vector_t MAX_POS = {2000, 1000};

const double G = 30;
const double MAX_RADIUS = 60;
const double MIN_RADIUS = 20;
const int POLYGON_COUNT = 60;
const int RAINBOW_STEP = 5;

typedef struct state {
  scene_t *scene;
  rgb_color_t *rainbow;
  size_t curr_color;
} state_t;

void generate_new_shape(state_t *state) {
  double spawnable_area = 3.0 / 4;

  double radius = (rand() % (int)(MAX_RADIUS - MIN_RADIUS)) + MIN_RADIUS;
  double mass = radius * radius;
  double centroid_x =
      (rand() % (int)((MAX_POS.x - MIN_POS.x) * spawnable_area)) + MIN_POS.x +
      (MAX_POS.x - MIN_POS.x) * (1 - spawnable_area) / 2;
  double centroid_y =
      (rand() % (int)((MAX_POS.y - MIN_POS.y) * spawnable_area)) + MIN_POS.y +
      (MAX_POS.y - MIN_POS.y) * (1 - spawnable_area) / 2;
  vector_t *centroid = malloc(sizeof(vector_t));
  *centroid = (vector_t){centroid_x, centroid_y};
  list_t *shape = draw_star(4, centroid, radius / 2, radius);
  rgb_color_t color = state->rainbow[state->curr_color];
  state->curr_color = (state->curr_color + 1) % (RAINBOW_STEP * 6);

  body_t *body = body_init(shape, mass, color);
  scene_add_body(state->scene, body);
}

void apply_forces(state_t *state) {
  size_t body_count = scene_bodies(state->scene);
  for (size_t i = 0; i < body_count; i++) {
    for (size_t j = i + 1; j < body_count; j++) {
      body_t *body1 = scene_get_body(state->scene, i);
      body_t *body2 = scene_get_body(state->scene, j);
      create_newtonian_gravity(state->scene, G, body1, body2);
    }
  }
}

state_t *emscripten_init() {
  state_t *state = malloc(sizeof(state_t));
  assert(state != NULL);
  sdl_init(MIN_POS, MAX_POS);
  state->scene = scene_init();
  state->rainbow = create_rainbow(RAINBOW_STEP);
  state->curr_color = 0;
  for (int i = 0; i < POLYGON_COUNT; i++) {
    generate_new_shape(state);
  }
  apply_forces(state);
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
