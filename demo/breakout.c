#include "body.h"
#include "color.h"
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
  PADDLE_INFO = 0,
  BALL_INFO = 1,
  BRICK_INFO = 2,
  WALL_INFO = 3
} info_t;

const double INTERVAL_BETWEEN_POWERUPS = 5;
const double GROWTH_FACTOR = 1.1;

const double BRICK_ELASTICITY = 1.02;
const double WALL_ELASTICITY = 1;
const double WALL_WIDTH = 20;

const double PADDLE_WIDTH = 100;
const double PADDLE_HEIGHT = 20;
const double PADDLE_MASS = INFINITY;
const rgb_color_t PADDLE_COLOR = {1, 0, 0};
const vector_t PADDLE_VELOCITY = {750, 0};

const vector_t BALL_INITIAL_VELOCITY = {750, 750};
const double BALL_RADIUS = 15;
const double BALL_MASS = 20;
const rgb_color_t BALL_COLOR = {1, 0, 0};

const int NUM_BRICK_ROWS = 3;
const int NUM_BRICK_COLUMNS = 10;
const int BRICK_PADDING = 10;
const double BRICK_HEIGHT = 40;
const double BRICK_MASS = INFINITY;

typedef struct state {
  scene_t *scene;
  body_t *paddle;
  bool is_done;
  double time;
  rgb_color_t *rainbow;
  int rainbow_step;
  double paddle_width;
} state_t;

void walls_init(state_t *state) {
  vector_t *U_centroid = malloc(sizeof(vector_t));
  *U_centroid =
      (vector_t){0.5 * (MAX_POS.x - MIN_POS.x), MAX_POS.y + WALL_WIDTH * 0.5};
  int *info_u = malloc(sizeof(int));
  *info_u = WALL_INFO;
  body_t *U_wall =
      body_init_with_info(draw_rectangle(U_centroid, MAX_POS.x, WALL_WIDTH),
                          INFINITY, WHITE, info_u, free);
  scene_add_body(state->scene, U_wall);

  vector_t *R_centroid = malloc(sizeof(vector_t));
  *R_centroid =
      (vector_t){0.5 * WALL_WIDTH + MAX_POS.x, (MAX_POS.y - MIN_POS.y) * 0.5};
  int *info_r = malloc(sizeof(int));
  *info_r = WALL_INFO;
  body_t *R_wall =
      body_init_with_info(draw_rectangle(R_centroid, WALL_WIDTH, MAX_POS.y),
                          INFINITY, WHITE, info_r, free);
  scene_add_body(state->scene, R_wall);

  vector_t *L_centroid = malloc(sizeof(vector_t));
  *L_centroid = (vector_t){-0.5 * WALL_WIDTH, (MAX_POS.y - MIN_POS.y) * 0.5};
  int *info_l = malloc(sizeof(int));
  *info_l = WALL_INFO;
  body_t *L_wall =
      body_init_with_info(draw_rectangle(L_centroid, WALL_WIDTH, MAX_POS.y),
                          INFINITY, WHITE, info_l, free);
  scene_add_body(state->scene, L_wall);
}

void paddle_init(state_t *state) {
  vector_t *initial_centroid = malloc(sizeof(vector_t));
  *initial_centroid = (vector_t){0.5 * (MAX_POS.x - MIN_POS.x),
                                 MIN_POS.y + 0.8 * PADDLE_HEIGHT};
  int *info = malloc(sizeof(int));
  *info = PADDLE_INFO;
  state->paddle = body_init_with_info(
      draw_rectangle(initial_centroid, PADDLE_WIDTH, PADDLE_HEIGHT),
      PADDLE_MASS, PADDLE_COLOR, info, free);
  scene_add_body(state->scene, state->paddle);
}

void bricks_init(state_t *state) {
  double brick_width =
      ((MAX_POS.x - MIN_POS.x) - ((NUM_BRICK_COLUMNS + 1) * BRICK_PADDING)) /
      (NUM_BRICK_COLUMNS);
  rgb_color_t *brick_color = create_rainbow(NUM_BRICK_COLUMNS);
  for (int i = 0; i < NUM_BRICK_ROWS; i++) {
    for (int j = 0; j < NUM_BRICK_COLUMNS; j++) {
      double initial_x =
          BRICK_PADDING + (brick_width / 2) + j * (brick_width + BRICK_PADDING);
      double initial_y = MAX_POS.y - BRICK_PADDING - (BRICK_HEIGHT / 2) -
                         (i * (BRICK_HEIGHT + BRICK_PADDING));
      vector_t *centroid = malloc(sizeof(vector_t));
      *centroid = (vector_t){initial_x, initial_y};
      int *info = malloc(sizeof(int));
      *info = BRICK_INFO;
      body_t *brick = body_init_with_info(
          draw_rectangle(centroid, BRICK_HEIGHT, brick_width), BRICK_MASS,
          brick_color[j * 6], info, free);
      body_rotate_about_point(brick, -M_PI / 2, *centroid);
      scene_add_body(state->scene, brick);
    }
  }
}

void ball_init(state_t *state) {
  vector_t centroid =
      vec_add(body_get_centroid(state->paddle), (vector_t){0, 2 * BALL_RADIUS});
  int *info = malloc(sizeof(int));
  *info = BALL_INFO;
  body_t *new_ball = body_init_with_info(draw_circle(&centroid, BALL_RADIUS),
                                         BALL_MASS, BALL_COLOR, info, free);
  body_set_velocity(new_ball, BALL_INITIAL_VELOCITY);
  scene_add_body(state->scene, new_ball);
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *curr = scene_get_body(state->scene, i);
    int info = *(int *)body_get_info(curr);
    if (info == WALL_INFO || info == PADDLE_INFO) {
      create_physics_collision(state->scene, WALL_ELASTICITY, new_ball, curr);
    } else if (info == BRICK_INFO) {
      create_breaking_collision(state->scene, BRICK_ELASTICITY, new_ball, curr);
    }
  }
}

void on_key(char key, button_event_type_t type, double held_time,
            state_t *state) {
  if (!state->is_done) {
    if (type == BUTTON_PRESSED) {
      switch (key) {
      case RIGHT_ARROW:
        body_set_velocity(state->paddle, PADDLE_VELOCITY);
        break;
      case LEFT_ARROW:
        body_set_velocity(state->paddle, vec_negate(PADDLE_VELOCITY));
        break;
      }
    } else if (type == BUTTON_RELEASED &&
               (key == RIGHT_ARROW || key == LEFT_ARROW)) {
      body_set_velocity(state->paddle, VEC_ZERO);
    }
  }
}

void compute_positions(state_t *state) {
  bool brick_exists = false;
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *curr = scene_get_body(state->scene, i);
    vector_t centroid = body_get_centroid(curr);
    switch (*(int *)body_get_info(curr)) {
    case PADDLE_INFO:
      if (centroid.x >= MAX_POS.x - state->paddle_width / 2) {
        body_set_centroid(
            curr, (vector_t){MAX_POS.x - state->paddle_width / 2, centroid.y});
      } else if (centroid.x <= MIN_POS.x + state->paddle_width / 2) {
        body_set_centroid(
            curr, (vector_t){MIN_POS.x + state->paddle_width / 2, centroid.y});
      }
      break;
    case BALL_INFO:
      if (centroid.y < MIN_POS.y - BALL_RADIUS) {
        emscripten_free(state);
        state = emscripten_init();
        goto end;
      }
      break;
    case BRICK_INFO:
      brick_exists = true;
      break;
    }
  }
  if (!brick_exists) {
    state->is_done = true;
    exit(0);
  }
end:;
}

state_t *emscripten_init() {
  state_t *state = malloc(sizeof(state_t));
  sdl_init(MIN_POS, MAX_POS);
  sdl_on_key((key_handler_t)on_key);
  state->scene = scene_init();
  walls_init(state);
  paddle_init(state);
  bricks_init(state);
  ball_init(state);
  state->rainbow = create_rainbow(NUM_BRICK_COLUMNS);
  state->rainbow_step = 0;
  state->is_done = false;
  state->paddle_width = PADDLE_WIDTH;
  return state;
}

void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  scene_tick(state->scene, dt);
  state->time += dt;
  if (state->time >= INTERVAL_BETWEEN_POWERUPS &&
      state->paddle_width <= (MAX_POS.x - MIN_POS.x) / GROWTH_FACTOR) {
    body_stretch_x(state->paddle, GROWTH_FACTOR);
    body_set_color(state->paddle, state->rainbow[state->rainbow_step]);
    state->paddle_width *= GROWTH_FACTOR;
    state->rainbow_step = (state->rainbow_step + 3) % (NUM_BRICK_COLUMNS * 6);
    state->time = 0;
  }
  sdl_render_scene(state->scene);
  compute_positions(state);
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state->rainbow);
  free(state->paddle);
  free(state);
}
