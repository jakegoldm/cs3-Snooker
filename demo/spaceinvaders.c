#include "body.h"
#include "forces.h"
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
const vector_t MAX_POS = {1000, 1000};

typedef enum {
  SPACESHIP_INFO = 0,
  SPACESHIP_AMMO_INFO = 1,
  ENEMY_INFO = 2,
  ENEMY_AMMO_INFO = 3
} info_t;

const double SPACESHIP_PADDING = 5;
const double MAJOR = 60;
const double MINOR = 20;
const double SPACESHIP_MASS = 50;
const rgb_color_t SPACESHIP_COLOR = {0.40, 0.60, 0.80};
const vector_t SPACESHIP_VELOCITY = {750, 0};

const int ENEMIES_PER_ROW = 8;
const int NUM_ENEMY_ROWS = 3;
const double ENEMY_PADDING = 25;
const double ENEMY_OFFSET = 15;
const double ENEMY_MASS = 100;
const rgb_color_t ENEMY_COLOR = {0.5, 0.5, 0.5};
const double ENEMY_ANGLE = 7 * M_PI / 6;
const double ENEMY_VELOCITY = 100;

const double AMMO_WIDTH = 10;
const double AMMO_HEIGHT = 40;
const double AMMO_MASS = 5;
const int MAX_NUM_AMMO = 10;
const double TIME_BETWEEN_AMMO = 1;
const double AMMO_VELOCITY = 1000;
const rgb_color_t SPACESHIP_AMMO_COLOR = {0.22, 1, 0.08};
const rgb_color_t ENEMY_AMMO_COLOR = {0.3, 0.3, 0.3};

typedef struct state {
  scene_t *scene;
  body_t *spaceship;
  bool is_done;
} state_t;

double radius() { return (MAX_POS.x - MIN_POS.x) / (2 * (ENEMIES_PER_ROW)); }

void spaceship_init(state_t *state) {
  vector_t *initial_centroid = malloc(sizeof(vector_t));
  *initial_centroid = (vector_t){0.5 * (MAX_POS.x - MIN_POS.x),
                                 MIN_POS.y + SPACESHIP_PADDING + MINOR};
  int *info = malloc(sizeof(int));
  *info = SPACESHIP_INFO;
  state->spaceship =
      body_init_with_info(draw_ellipse(initial_centroid, MAJOR, MINOR),
                          SPACESHIP_MASS, SPACESHIP_COLOR, info, free);
  scene_add_body(state->scene, state->spaceship);
}

void enemies_init(state_t *state) {
  for (int i = 0; i < NUM_ENEMY_ROWS; i++) {
    for (int j = 0; j < ENEMIES_PER_ROW; j++) {
      double initial_x = (2 * j + 1) * radius() + 1;
      double initial_y = MAX_POS.y - ((i + 1) * (radius() + ENEMY_PADDING));
      vector_t *centroid = malloc(sizeof(vector_t));
      *centroid = (vector_t){initial_x, initial_y};
      int *info = malloc(sizeof(int));
      *info = ENEMY_INFO;
      body_t *enemy = body_init_with_info(
          draw_arc_offset(centroid, radius(), 3 * M_PI / 2 - ENEMY_ANGLE / 2,
                          3 * M_PI / 2 + ENEMY_ANGLE / 2, 0, -ENEMY_OFFSET),
          ENEMY_MASS, ENEMY_COLOR, info, free);
      body_set_velocity(enemy, (vector_t){ENEMY_VELOCITY, 0});
      scene_add_body(state->scene, enemy);
    }
  }
}

void enemy_ammo_init(state_t *state) {
  bool is_enemy = false;
  vector_t centroid = VEC_ZERO;
  while (!is_enemy) {
    int index = rand() % scene_bodies(state->scene);
    body_t *curr = scene_get_body(state->scene, index);
    if (*(int *)body_get_info(curr) == ENEMY_INFO) {
      centroid = body_get_centroid(curr);
      is_enemy = true;
    }
  }
  vector_t ammo_centroid = {centroid.x, centroid.y - ENEMY_PADDING};
  int *info = malloc(sizeof(int));
  *info = ENEMY_AMMO_INFO;
  body_t *new_ammo = body_init_with_info(
      draw_rectangle(&ammo_centroid, AMMO_WIDTH, AMMO_HEIGHT), AMMO_MASS,
      ENEMY_AMMO_COLOR, info, free);
  body_set_velocity(new_ammo, (vector_t){0, -AMMO_VELOCITY});
  scene_add_body(state->scene, new_ammo);
  create_destructive_collision(state->scene, state->spaceship, new_ammo);
}

void spaceship_ammo_init(state_t *state) {
  vector_t centroid = body_get_centroid(state->spaceship);
  int *info = malloc(sizeof(int));
  *info = ENEMY_AMMO_INFO;
  body_t *new_ammo =
      body_init_with_info(draw_rectangle(&centroid, AMMO_WIDTH, AMMO_HEIGHT),
                          AMMO_MASS, SPACESHIP_AMMO_COLOR, info, free);
  body_set_velocity(new_ammo, (vector_t){0, AMMO_VELOCITY});
  scene_add_body(state->scene, new_ammo);
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *curr = scene_get_body(state->scene, i);
    if (*(int *)body_get_info(curr) == ENEMY_INFO) {
      create_destructive_collision(state->scene, new_ammo, curr);
    }
  }
}

void on_key(char key, button_event_type_t type, double held_time,
            state_t *state) {
  if (!state->is_done) {
    if (type == BUTTON_PRESSED) {
      switch (key) {
      case ' ':
        spaceship_ammo_init(state);
        break;
      case RIGHT_ARROW:
        body_set_velocity(state->spaceship, SPACESHIP_VELOCITY);
        break;
      case LEFT_ARROW:
        body_set_velocity(state->spaceship, vec_negate(SPACESHIP_VELOCITY));
        break;
      }
    } else if (type == BUTTON_RELEASED &&
               (key == RIGHT_ARROW || key == LEFT_ARROW)) {
      body_set_velocity(state->spaceship, VEC_ZERO);
    }
  }
}

void compute_positions(state_t *state) {
  bool spaceship_exists = false;
  bool enemy_exists = false;
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *curr = scene_get_body(state->scene, i);
    vector_t centroid = body_get_centroid(curr);
    switch (*(int *)body_get_info(curr)) {
    case SPACESHIP_INFO:
      spaceship_exists = true;
      if (centroid.x >= MAX_POS.x - MAJOR) {
        body_set_centroid(curr, (vector_t){MAX_POS.x - MAJOR, centroid.y});
      } else if (centroid.x <= MIN_POS.x + MAJOR) {
        body_set_centroid(curr, (vector_t){MIN_POS.x + MAJOR, centroid.y});
      }
      break;
    case SPACESHIP_AMMO_INFO:
      if (centroid.y > MAX_POS.y + AMMO_HEIGHT) {
        body_remove(curr);
      }
      break;
    case ENEMY_INFO:
      enemy_exists = true;
      if (centroid.y <= MIN_POS.y - radius()) {
        state->is_done = true;
        exit(0);
      } else {
        if (centroid.x < MIN_POS.x + radius()) {
          body_set_centroid(
              curr, (vector_t){MIN_POS.x + radius() + 1,
                               centroid.y - NUM_ENEMY_ROWS *
                                                (radius() + ENEMY_PADDING)});
          body_set_velocity(curr, vec_negate(body_get_velocity(curr)));
        } else if (centroid.x > MAX_POS.x - radius()) {
          body_set_centroid(
              curr, (vector_t){MAX_POS.x - radius() - 1,
                               centroid.y - NUM_ENEMY_ROWS *
                                                (radius() + ENEMY_PADDING)});
          body_set_velocity(curr, vec_negate(body_get_velocity(curr)));
        }
      }
      break;
    case ENEMY_AMMO_INFO:
      if (centroid.y < MIN_POS.y - AMMO_HEIGHT) {
        body_remove(curr);
      }
      break;
    }
  }
  if (!spaceship_exists || !enemy_exists) {
    state->is_done = true;
    exit(0);
  }
}

state_t *emscripten_init() {
  state_t *state = malloc(sizeof(state_t));
  sdl_init(MIN_POS, MAX_POS);
  sdl_on_key((key_handler_t)on_key);
  state->scene = scene_init();
  spaceship_init(state);
  enemies_init(state);
  state->is_done = false;
  return state;
}

void emscripten_main(state_t *state) {
  if (scene_get_time(state->scene) >= TIME_BETWEEN_AMMO) {
    scene_reset_time(state->scene);
    enemy_ammo_init(state);
  }
  compute_positions(state);
  scene_tick(state->scene, time_since_last_tick());
  sdl_render_scene(state->scene);
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}
