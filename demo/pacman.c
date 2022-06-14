#include "body.h"
#include "list.h"
#include "polygon.h"
#include "scene.h"
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

const double PASSIVE_VELOCITY = 200;
const double ACCELERATION = 200;
const double MAX_BODIES = 101;

const int INITIAL_PELLETS = 30;
const double PELLET_RADIUS = 15;
const double PELLET_MASS = 1.0;
const rgb_color_t PELLET_COLOR = {0.45, 0.65, 0.75};
const double TIME_BETWEEN_PELLETS = 0.5;

const double PACMAN_MASS = 10.0;
const double PACMAN_RADIUS = 100;
const rgb_color_t PACMAN_COLOR = {1, 1, 0};
const double MOUTH_ANGLE = M_PI / 3;

typedef struct state {
  scene_t *scene;
  body_t *pacman;
  rgb_color_t *rainbow;
} state_t;

void pacman_init(state_t *state) {
  vector_t *initial_centroid = malloc(sizeof(vector_t));
  *initial_centroid =
      (vector_t){0.5 * (MAX_POS.x - MIN_POS.x), 0.5 * (MAX_POS.y - MIN_POS.y)};
  state->pacman = body_init(draw_arc(initial_centroid, PACMAN_RADIUS,
                                     -MOUTH_ANGLE / 2, MOUTH_ANGLE / 2),
                            PACMAN_MASS, PACMAN_COLOR);
  body_set_velocity(state->pacman, VEC_ZERO);
  scene_add_body(state->scene, state->pacman);
}

void draw_pellet(state_t *state) {
  vector_t *centroid = malloc(sizeof(vector_t));
  *centroid =
      (vector_t){PELLET_RADIUS + rand() % (int)(MAX_POS.x - 2 * PELLET_RADIUS),
                 PELLET_RADIUS + rand() % (int)(MAX_POS.y - 2 * PELLET_RADIUS)};
  size_t color_i = rand() % (INITIAL_PELLETS * 6);
  rgb_color_t color = state->rainbow[color_i];
  body_t *pellet =
      body_init(draw_circle(centroid, PELLET_RADIUS), PELLET_MASS, color);
  scene_add_body(state->scene, pellet);
}

void accelerate(double angle, double held_time, body_t *pacman) {
  if (held_time == 0) {
    body_set_rotation(pacman, angle);
  }
  body_set_velocity(
      pacman, vec_init(ACCELERATION * held_time + PASSIVE_VELOCITY, angle));
}

void compute_new_positions(state_t *state, double dt) {
  vector_t centroid = body_get_centroid(state->pacman);
  if (centroid.x > MAX_POS.x + PACMAN_RADIUS) {
    body_set_centroid(state->pacman,
                      (vector_t){MIN_POS.x - PACMAN_RADIUS, centroid.y});
  } else if (centroid.x < MIN_POS.x - PACMAN_RADIUS) {
    body_set_centroid(state->pacman,
                      (vector_t){MAX_POS.x + PACMAN_RADIUS, centroid.y});
  } else if (centroid.y > MAX_POS.y + PACMAN_RADIUS) {
    body_set_centroid(state->pacman,
                      (vector_t){centroid.x, MIN_POS.y - PACMAN_RADIUS});
  } else if (centroid.y < MIN_POS.y - PACMAN_RADIUS) {
    body_set_centroid(state->pacman,
                      (vector_t){centroid.x, MAX_POS.y + PACMAN_RADIUS});
  }
}

void on_key(char key, button_event_type_t type, double held_time,
            state_t *state) {
  if (type == BUTTON_PRESSED) {
    switch (key) {
    case UP_ARROW:
      accelerate(M_PI / 2, held_time, state->pacman);
      break;
    case DOWN_ARROW:
      accelerate(3 * M_PI / 2, held_time, state->pacman);
      break;
    case RIGHT_ARROW:
      accelerate(0, held_time, state->pacman);
      break;
    case LEFT_ARROW:
      accelerate(M_PI, held_time, state->pacman);
      break;
    }
  } else if (type == BUTTON_RELEASED) {
    body_set_velocity(
        state->pacman,
        vec_init(PASSIVE_VELOCITY,
                 vec_direction(body_get_velocity(state->pacman))));
  }
}

void eat_pellet(state_t *state) {
  vector_t pacman_centroid = body_get_centroid(state->pacman);
  for (int i = 0; i < scene_bodies(state->scene); i++) {
    body_t *curr = scene_get_body(state->scene, i);
    if (curr != state->pacman) {
      vector_t pellet_centroid =
          body_get_centroid(scene_get_body(state->scene, i));
      if (vec_magnitude(vec_subtract(pellet_centroid, pacman_centroid)) <=
          PACMAN_RADIUS + PELLET_RADIUS) {
        scene_remove_body(state->scene, i);
      }
    }
  }
}

state_t *emscripten_init() {
  state_t *state = malloc(sizeof(state_t));
  state->scene = scene_init();
  sdl_init(MIN_POS, MAX_POS);
  state->rainbow = create_rainbow(INITIAL_PELLETS);
  pacman_init(state);
  sdl_on_key((key_handler_t)on_key);
  for (int i = 0; i < INITIAL_PELLETS; i++) {
    draw_pellet(state);
  }
  return state;
}

void emscripten_main(state_t *state) {
  if (scene_get_time(state->scene) >= TIME_BETWEEN_PELLETS &&
      scene_bodies(state->scene) < MAX_BODIES) {
    scene_reset_time(state->scene);
    draw_pellet(state);
  }
  double dt = time_since_last_tick();
  scene_tick(state->scene, dt);
  compute_new_positions(state, dt);
  eat_pellet(state);
  sdl_render_scene(state->scene);
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}
