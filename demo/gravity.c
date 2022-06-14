#include "body.h"
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
const vector_t INIT_VELOCITY = {250, 0};

const double ANGLE = M_PI / 4;
const double MIN_ELASTICITY = 0.70;
const double GRAVITY = -9.81 * 40;

const double OUTER_RADIUS = 120;
const double INNER_RADIUS = 60;
const int MAX_POLYGON_COUNT = 7;
const int MAX_N = 60;
const int RAINBOW_STEP = 3;

typedef struct state {
  list_t *shapes;
  list_t *elasticities;
  list_t *angles;
  rgb_color_t *rainbow;
  double time;
  int curr_n;
} state_t;

void compute_new_positions(body_t *shape, double dt, double elasticity,
                           double angle) {
  body_rotate(shape, dt * angle);
  body_tick(shape, dt);
  vector_t centroid = body_get_centroid(shape);
  vector_t velocity = body_get_velocity(shape);
  velocity.y += GRAVITY * dt;
  list_t *points = body_get_shape(shape);
  size_t num_points = list_size(points);
  if (num_points < 24) {
    for (size_t i = 0; i < list_size(points); i++) {
      vector_t curr = *(vector_t *)list_get(points, i);
      if (curr.y <= MIN_POS.y) {
        velocity.y *= -elasticity;
        body_set_centroid(
            shape, (vector_t){centroid.x, centroid.y + MIN_POS.y - curr.y});
        break;
      }
    }
  } else {
    if (centroid.y < MIN_POS.y + OUTER_RADIUS) {
      velocity.y *= -elasticity;
      body_set_centroid(shape,
                        (vector_t){centroid.x, MIN_POS.y + OUTER_RADIUS});
    }
  }
  body_set_velocity(shape, velocity);
  list_free(points);
}

void generate_new_shape(state_t *state) {
  state->time = 0;
  rgb_color_t rainbow_color =
      state->rainbow[(state->curr_n - 2) % (6 * RAINBOW_STEP)];
  double *elasticity = malloc(sizeof(double));
  *elasticity = 0.01 * ((rand() % (91 - (int)(MIN_ELASTICITY * 100))) +
                        MIN_ELASTICITY * 100);
  double *angle = malloc(sizeof(double));
  *angle = ANGLE * (0.5 + (rand() % 100) / 66.0);
  vector_t *origin = malloc(sizeof(vector_t));
  *origin = (vector_t){MIN_POS.x, MAX_POS.y};
  list_t *points = draw_star(state->curr_n, origin, INNER_RADIUS, OUTER_RADIUS);
  free(origin);
  body_t *shape = body_init(points, 1, rainbow_color);
  body_set_velocity(shape, INIT_VELOCITY);
  list_add(state->shapes, shape);
  list_add(state->elasticities, elasticity);
  list_add(state->angles, angle);
  state->curr_n++;
}

void display_shapes(state_t *state, double dt) {
  for (int i = 0; i < (int)list_size(state->shapes); i++) {
    body_t *curr_shape = list_get(state->shapes, i);
    double curr_elasticity = *(double *)list_get(state->elasticities, i);
    double curr_angle = *(double *)list_get(state->angles, i);
    compute_new_positions(curr_shape, dt, curr_elasticity, curr_angle);
    sdl_draw_polygon(curr_shape);
  }
}

state_t *emscripten_init() {
  state_t *state = malloc(sizeof(state_t));
  assert(state != NULL);
  sdl_init(MIN_POS, MAX_POS);
  state->shapes = list_init(MAX_POLYGON_COUNT, (free_func_t)list_free);
  state->elasticities = list_init(MAX_POLYGON_COUNT, (free_func_t)free);
  state->angles = list_init(MAX_POLYGON_COUNT, (free_func_t)free);
  state->rainbow = create_rainbow(RAINBOW_STEP);
  state->time = 0;
  state->curr_n = 2;
  return state;
}

void emscripten_main(state_t *state) {
  if (state->curr_n <= MAX_N &&
      (list_size(state->shapes) == 0 ||
       state->time >= (MAX_POS.x + OUTER_RADIUS) /
                          (MAX_POLYGON_COUNT * INIT_VELOCITY.x))) {
    if (list_size(state->shapes) >= MAX_POLYGON_COUNT) {
      list_remove(state->shapes, 0);
      list_remove(state->elasticities, 0);
      list_remove(state->angles, 0);
    }
    generate_new_shape(state);
  }
  double dt = time_since_last_tick();
  state->time += dt;
  sdl_clear();
  display_shapes(state, dt);
  sdl_show();
}

void emscripten_free(state_t *state) {
  list_free(state->shapes);
  list_free(state->elasticities);
  list_free(state->angles);
  free(state);
}
