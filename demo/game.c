#include "forces.h"
#include "game_state.h"
#include "menu_state.h"
#include "polygon.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "shape.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * http://shurikencues.com/shop/classic/classic-fullsplice-cue-made-lapacho-ipe-hornbeam/
 * https://dynamicbilliard.ca/resources/snooker-table-layout/
 * https://www.dimensions.com/element/billiard-balls
 * https://www.dimensions.com/element/billiards-pool-table-pockets
 */

bool ball_within(scene_t *scene, vector_t centroid, body_t *ball) {
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_t *curr = scene_get_body(scene, i);
    if (*(int *)body_get_info(curr) > BLACK_INFO) {
      continue;
    }
    double distance =
        vec_magnitude(vec_subtract(centroid, body_get_centroid(curr)));
    if (curr != ball && distance <= 2 * ball_radius()) {
      return true;
    }
  }
  return false;
}

void respawn_ball(scene_t *scene, body_t *ball, info_t info) {
  vector_t ball_pos[6] = {yellow_pos(), green_pos(), brown_pos(),
                          blue_pos(),   pink_pos(),  black_pos()};
  vector_t centroid = ball_pos[info - 2];
  body_hide(ball, false);
  body_set_to_respawn(ball, false);
  if (!ball_within(scene, centroid, ball)) {
    body_set_centroid(ball, centroid);
    return;
  }
  while (info < BLACK_INFO) {
    if (!ball_within(scene, ball_pos[info - 1], ball)) {
      body_set_centroid(ball, ball_pos[info - 1]);
      return;
    }
    info++;
  }
  double increase = ball_radius();
  do {
    centroid.x += increase;
  } while (ball_within(scene, centroid, ball));
  body_set_centroid(ball, centroid);
}

void respawn_cue_ball(state_t *state) {
  vector_t centroid = {(MAX_POS.x - MIN_POS.x) / 2,
                       (MAX_POS.y - MIN_POS.y) / 2};
  body_set_centroid(state->cue_ball, centroid);
  body_hide(state->cue_ball, false);
  body_set_to_respawn(state->cue_ball, false);
  body_set_apply_forces(state->cue_ball, false);
  state->flags = SCRATCH | HIT;
}

void reset_cue(state_t *state) {
  body_set_velocity(state->cue, VEC_ZERO);
  body_set_rotation(state->cue, 0);
  body_set_centroid(state->cue, cue_pos());
  body_set_to_respawn(state->cue, false);
  body_set_apply_forces(state->cue, true);
}

void update_scores(state_t *state) {
  if (state->foul || !state->points) {
    state->player = (state->player + 1) % 2;
    state->scores[state->player] += fmax(state->foul, state->points);
    state->foul = 0;
    if (state->reds_left) {
      state->ball_on = 1;
    } else if (state->ball_on < 2) {
      state->ball_on = 2;
    }
  } else {
    state->scores[state->player] += state->points;
    if (state->reds_left) {
      state->ball_on = (state->ball_on + 1) % 2;
    } else if (state->ball_on < 7) {
      state->ball_on++;
    }
  }
  state->points = 0;
  state->first_hit = true;
}

void game_over(state_t *state) {
  int winner = state->scores[0] > state->scores[1] ? 1 : 2;
  printf("Game Over! Player %d wins %d to %d!\n", winner, state->scores[0],
         state->scores[1]);
}

bool tie(state_t *state, body_t *ball) {
  if (*(int *)body_get_info(ball) != BLACK_INFO) {
    return false;
  }
  if (state->scores[state->player] + BLACK_INFO !=
      state->scores[(state->player + 1) % 2]) {
    return false;
  }
  respawn_ball(state->scene, ball, BLACK_INFO);
  body_set_centroid(state->cue_ball, cue_ball_pos());
  state->flags |= SET_CUE_BALL;
  return true;
}

void compute_positions(state_t *state) {
  bool is_still = scene_is_still(state->scene), reds_left = false,
       balls_left = false;
  for (size_t i = 0; i < scene_bodies(state->scene); i++) {
    body_t *curr = scene_get_body(state->scene, i);
    info_t *info = body_get_info(curr);
    vector_t velocity = body_get_velocity(curr);
    switch (*info) {
    case CUE_BALL_INFO:
      if (body_to_respawn(curr)) {
        if (is_still) {
          respawn_cue_ball(state);
        } else {
          body_hide(curr, true);
          body_set_velocity(curr, VEC_ZERO);
        }
      } else if (is_still) {
        ball_in_table(curr);
      }
      break;
    case RED_INFO:
      reds_left = true;
      break;
    case YELLOW_INFO:
    case GREEN_INFO:
    case BROWN_INFO:
    case BLUE_INFO:
    case PINK_INFO:
    case BLACK_INFO:
      balls_left = true;
      if (body_to_respawn(curr)) {
        if (is_still) {
          if (state->reds_left || state->foul) {
            respawn_ball(state->scene, curr, *info);
          } else if (!tie(state, curr)) {
            body_remove(curr);
          }
        } else {
          body_hide(curr, true);
          body_set_velocity(curr, VEC_ZERO);
        }
      }
      break;
    case CUE_INFO:
      if (body_to_respawn(curr)) {
        if (vec_magnitude(velocity) < 1e0) {
          reset_cue(state);
        } else {
          body_set_velocity(curr, vec_multiply(0.8, body_get_velocity(curr)));
        }
      }
      break;
    case WALL_INFO:
      break;
    case POCKET_INFO:
      break;
    }
  }
  if (is_still) {
    state->flags |= CUE_HIT;
    state->reds_left = reds_left;
    if (state->flags & HIT) {
      if (state->first_hit) {
        state->foul = fmax(state->ball_on, 4);
      }
      update_scores(state);
      state->flags ^= HIT;
      printf("Player %d to hit: %d vs %d\n", state->player + 1,
             state->scores[0], state->scores[1]);
    }
    if (!balls_left) {
      game_over(state);
    }
  }
}

void move_cue_ball(state_t *state, double direction) {
  bool contained;
  if (state->flags & SET_CUE_BALL) {
    contained = ball_in_semicircle(state->cue_ball);
  } else if (state->flags & SCRATCH) {
    contained = ball_in_table(state->cue_ball);
  } else {
    return;
  }
  if (state->flags & POWER_METER) {
    reset_cue(state);
    remove_dotted_lines(state);
    state->flags ^= POWER_METER;
  }
  body_set_apply_forces(state->cue_ball, false);
  if (contained) {
    body_set_velocity(state->cue_ball, vec_init(SCRATCH_VELOCTY, direction));
  } else {
    body_set_velocity(state->cue_ball, VEC_ZERO);
  }
}

void menu_on_click(int button, button_event_type_t type, vector_t position,
                   double held_time, state_t *state) {
  if (type == BUTTON_PRESSED) {
    switch (button) {}
  } else if (type == BUTTON_RELEASED) {
    switch (button) {
    case LEFT_CLICK: {
      if (on_start_button(position) && state->in_alt_state == false) {
        state_goto_next_state(state);
      }
      if (on_rules_button(position) && state->in_alt_state == false) {
        state_goto_alt_state(state);
      }
      if (on_reset_button(position) && state->in_alt_state == true) {
        emscripten_free(state);
        state = emscripten_init();
        return;
      }
    }
    }
  }
}

void game_on_key(char key, button_event_type_t type, double held_time,
                 state_t *state) {
  if (type == BUTTON_PRESSED) {
    switch (key) {
    case UP_ARROW:
      move_cue_ball(state, M_PI / 2);
      break;
    case DOWN_ARROW:
      move_cue_ball(state, -M_PI / 2);
      break;
    case RIGHT_ARROW:
      move_cue_ball(state, 0);
      break;
    case LEFT_ARROW:
      move_cue_ball(state, M_PI);
      break;
    case 'c':
      state->chalk[state->player] = 1;
      break;
    case ' ':
      if (state->flags & POWER_METER) {
        body_hide(state->slider, false);
        double amplitude = power_bar_height() * (1 - 2 * POWER_SLIDER_OFFSET);
        vector_t axis = vec_unit(vec_subtract(
            body_get_centroid(state->cue), body_get_centroid(state->cue_ball)));
        double speed = fmod(held_time, 2 * CUE_PERIOD) > CUE_PERIOD
                           ? -amplitude / CUE_PERIOD
                           : amplitude / CUE_PERIOD;
        body_set_velocity(state->slider, (vector_t){0, speed});
        body_set_velocity(state->cue, vec_multiply(speed / 2, axis));
        state->flags = POWER_METER;
      }
      break;
    }
  } else if (type == BUTTON_RELEASED) {
    switch (key) {
    case UP_ARROW:
    case DOWN_ARROW:
    case RIGHT_ARROW:
    case LEFT_ARROW:
      if (state->flags & SET_CUE_BALL) {
        ball_in_semicircle(state->cue_ball);
        body_set_velocity(state->cue_ball, VEC_ZERO);
      } else if (state->flags & SCRATCH) {
        ball_in_table(state->cue_ball);
        body_set_velocity(state->cue_ball, VEC_ZERO);
      }
      break;
    case ' ':
      if (state->flags == POWER_METER) {
        remove_dotted_lines(state);
        vector_t axis = vec_unit(vec_subtract(
            body_get_centroid(state->cue_ball), body_get_centroid(state->cue)));
        double power =
            sqrt(body_get_centroid(state->slider).y - slider_pos().y) /
            (power_bar_height() * (1 - 2 * POWER_SLIDER_OFFSET));
        body_set_velocity(state->cue,
                          vec_multiply(power * CUE_MAX_SPEED, axis));
        body_set_velocity(state->slider, VEC_ZERO);
        body_set_centroid(state->slider, slider_pos());
        body_hide(state->slider, true);
        state->flags ^= (POWER_METER | HIT);
      }
      break;
    }
  }
}

void game_on_click(int button, button_event_type_t type, vector_t position,
                   double held_time, state_t *state) {
  if (type == BUTTON_PRESSED) {
    switch (button) {}
  } else if (type == BUTTON_RELEASED) {
    switch (button) {
    case LEFT_CLICK: {
      if (state->flags & CUE_HIT && on_table(position)) {
        if (state->flags & (SET_CUE_BALL | SCRATCH) &&
            ball_within(state->scene, body_get_centroid(state->cue_ball),
                        state->cue_ball)) {
          break;
        }
        ball_in_table(state->cue_ball);
        body_set_apply_forces(state->cue_ball, true);
        vector_t cue_ball_pos = body_get_centroid(state->cue_ball);
        vector_t axis = vec_unit(vec_subtract(cue_ball_pos, position));
        vector_t new_center =
            vec_add(cue_ball_pos,
                    vec_multiply(cue_height() / 2 + 3 * ball_radius(), axis));
        body_set_rotation(state->cue, M_PI / 2 + vec_direction(axis));
        body_set_centroid(state->cue, new_center);
        remove_dotted_lines(state);
        create_dotted_lines(state, vec_negate(axis));
        state->flags |= POWER_METER;
      }
      if (on_reset_button(position)) {
        emscripten_free(state);
        state = emscripten_init();
        return;
      }
      if (on_mute_button(position)) {
        scene_toggle_muted(state->scene);
        game_state_toggle_mute(state);
      }
      break;
    }
    }
  }
}

void game_init(state_t *state) {
  sdl_on_click((mouse_handler_t)game_on_click);
  game_state_init(state);
}

state_t *emscripten_init(void) {
  srand(time(NULL));
  sdl_on_key((key_handler_t)game_on_key);
  sdl_on_click((mouse_handler_t)menu_on_click);
  return menu_state_init();
}

void emscripten_main(state_t *state) {
  double dt = time_since_last_tick();
  scene_tick(state->scene, dt);
  sdl_render_scene(state->scene);
  compute_positions(state);
  if (state->goto_next_state == true) {
    scene_free(state->scene);
    game_init(state);
  }
}

void emscripten_free(state_t *state) {
  scene_free(state->scene);
  free(state);
}
