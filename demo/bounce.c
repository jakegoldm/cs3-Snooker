#include "body.h"
#include "polygon.h"
#include "sdl_wrapper.h"
#include "state.h"
#include <assert.h>
#include <stdlib.h>

const vector_t MIN_POS = {0, 0};
const vector_t MAX_POS = {2000, 1000};
const size_t NUM_POINTS = 5;
const size_t SIDE_LEN = 150;
const vector_t INIT_POS = {1000, 500};
const vector_t INIT_VELOCITY = {1500, 1500};
const double INIT_ANGLE = M_PI;
const int RAINBOW_STEP = 100;

typedef struct state {
  body_t *polygon;
  rgb_color_t *rainbow;
  size_t color_i;
  float angle;
} state_t;

list_t *generate_star(size_t n, size_t len, vector_t center) {
  assert(n > 2);
  list_t *star = list_init(2 * n, (free_func_t)free);
  double theta = M_PI / n;

  // outer points
  list_t *temp_points = list_init(n, (free_func_t)free);
  for (int i = 1; i < 2 * n; i += 2) {
    vector_t *p = malloc(sizeof(vector_t));
    *p = (vector_t){center.x + len * sin(i * theta),
                    center.y - len * cos(i * theta)};
    list_add(temp_points, p);
  }

  // compute length of inner points
  double factor =
      fabs((((((vector_t *)list_get(temp_points, n / 2 + 1))->y -
              ((vector_t *)list_get(temp_points, 0))->y) /
             (((vector_t *)list_get(temp_points, n / 2 + 1))->x -
              ((vector_t *)list_get(temp_points, 0))->x)) *
                (center.x - ((vector_t *)list_get(temp_points, 0))->x) +
            ((vector_t *)list_get(temp_points, 0))->y - center.y) /
           len);

  for (int i = 0; i < 2 * n; i++) {
    double f =
        ((1 + i) % 2) * factor + i % 2; // apply the factor only to inner points
    vector_t *p = malloc(sizeof(vector_t));
    *p = (vector_t){center.x + f * len * sin(i * theta),
                    center.y - f * len * cos(i * theta)};
    list_add(star, p);
  }
  return star;
}

void compute_new_positions(state_t *state, double dt) {
  body_rotate(state->polygon, dt * state->angle);
  body_tick(state->polygon, dt);
  vector_t centroid = body_get_centroid(state->polygon);
  vector_t velocity = body_get_velocity(state->polygon);
  list_t *points = body_get_shape(state->polygon);
  for (int i = 0; i < list_size(points); i++) {
    vector_t curr = *(vector_t *)list_get(points, i);
    if (curr.x >= MAX_POS.x) {
      body_set_velocity(state->polygon,
                        (vector_t){velocity.x * -1, velocity.y});
      body_set_centroid(
          state->polygon,
          (vector_t){centroid.x + MAX_POS.x - curr.x, centroid.y});
      break;
    } else if (curr.x <= MIN_POS.x) {
      body_set_velocity(state->polygon,
                        (vector_t){velocity.x * -1, velocity.y});
      body_set_centroid(
          state->polygon,
          (vector_t){centroid.x + MIN_POS.x - curr.x, centroid.y});
      break;
    } else if (curr.y >= MAX_POS.y) {
      body_set_velocity(state->polygon,
                        (vector_t){velocity.x, velocity.y * -1});
      body_set_centroid(
          state->polygon,
          (vector_t){centroid.x, centroid.y + MAX_POS.y - curr.y});
      break;
    } else if (curr.y <= MIN_POS.y) {
      body_set_velocity(state->polygon,
                        (vector_t){velocity.x, velocity.y * -1});
      body_set_centroid(
          state->polygon,
          (vector_t){centroid.x, centroid.y + MIN_POS.y - curr.y});
      break;
    }
  }
  body_set_color(state->polygon, state->rainbow[state->color_i]);
  state->color_i = (state->color_i + 1) % (RAINBOW_STEP * 6);
  list_free(points);
}

state_t *emscripten_init() {
  state_t *state = malloc(sizeof(state_t));
  assert(state != NULL);
  sdl_init(MIN_POS, MAX_POS);

  // rgb_color_t color = {0.1, 0.5, 0.6};
  state->rainbow = create_rainbow(RAINBOW_STEP);
  state->color_i = 0;
  rgb_color_t color = state->rainbow[state->color_i];
  state->color_i++;
  state->angle = INIT_ANGLE;
  state->polygon =
      body_init(generate_star(NUM_POINTS, SIDE_LEN, INIT_POS), 1, color);
  body_set_velocity(state->polygon, INIT_VELOCITY);

  return state;
}

void emscripten_main(state_t *state) {
  compute_new_positions(state, time_since_last_tick());

  sdl_clear();
  sdl_draw_polygon(state->polygon);
  sdl_show();
}

void emscripten_free(state_t *state) {
  body_free(state->polygon);
  free(state->rainbow);
  free(state);
}
