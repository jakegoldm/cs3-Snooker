#ifndef __GAME_STATE_H__
#define __GAME_STATE_H__

/**
 * Header file defining all the constants, data types, etc
 * that will be used by game.c. It also contains all the
 * functions related to creating and resetting the game itself.
 */

#include "collision.h"
#include "forces.h"
#include "polygon.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "shape.h"
#include "state.h"

typedef enum {
  SET_CUE_BALL = 1,
  CUE_HIT = 2,
  POWER_METER = 4,
  SCRATCH = 8,
  HIT = 16,
} game_flags_t;

typedef enum {
  CUE_BALL_INFO = 0,
  RED_INFO = 1,
  YELLOW_INFO = 2,
  GREEN_INFO = 3,
  BROWN_INFO = 4,
  BLUE_INFO = 5,
  PINK_INFO = 6,
  BLACK_INFO = 7,
  POCKET_INFO = 8,
  WALL_INFO = 9,
  CUE_INFO = 10
} info_t;

typedef struct state {
  scene_t *scene;
  scene_t *game_scene, *main_menu, *in_game_menu; // to be implemented
  bool goto_next_state; // describes whether we should move to the next state
  bool in_alt_state; // describes if the current state is in its normal state,
                     // or an alternative, related state
  body_t *cue, *cue_ball, *slider, *reset_button, *mute_button, *start_button,
      *rules_button;
  list_t *semicircle, *table, *dotted_lines;
  game_flags_t flags;
  int player; // 0 and 1 for player 1 and 2
  int scores[2], foul, points,
      ball_on; // 1 for red, 0 for colors, 2+ for specific ball
  bool first_hit, reds_left, training_lines;
  double chalk[2];
  double time;
} state_t;

static const char TITLE[] = "CS 3: SNOOKER!";
static const vector_t MIN_POS = {0, 0};
static const vector_t MAX_POS = {4000, 2000};

static const double G = 980;
static const double MU = 0.35;
static const double CUE_ELASTICITY = 0.98;
static const double B_B_ELASTICITY = 0.9;
static const double B_W_ELASTICITY = 0.7;
static const double CHALK_DECAY_RATE = 0.05;
static const double MINIMUM_CHALK = 0.4;

static const double SCALE = 17;
static const double TABLE_WIDTH = 72;
static const double TABLE_HEIGHT = 144;
static const double TABLE_MASS = 3000;
static const double WALL_WIDTH = 10;
static const double EDGE_WIDTH = 2;
static const double POCKET_WIDTH = 6;
static const double POWER_BAR_HEIGHT = 50;
static const double POWER_BAR_WIDTH = 12;
static const double BUTTON_PADDING = 100;
static const double BUTTON_RADIUS = 60;

static const double CUE_HEIGHT = 59;
static const double CUE_WIDTH = 1.5;
static const double CUE_MASS = 550;
static const double BALL_RADIUS = 1.5;
static const double BALL_MASS = 140;
static const double SCRATCH_VELOCTY = 500;
static const double POWER_MARKER_HEIGHT = 1;
static const double POWER_MARKER_WIDTH = 10;
static const double POWER_SLIDER_OFFSET = 0.0713;
static const double CUE_PERIOD = 1.5;
static const double CUE_MAX_SPEED = 1500;

static const double LINE_WIDTH = 10;
static const double LINE_LENGTH = 30;
static const double MAX_COLLISIONS = 3;
static const double BALK_OFFSET = 29;
static const double SEMICIRCLE_RADIUS = 11.5;
static const double BLUE_OFFSET = 72;
static const double PINK_OFFSET = 144 - 40;
static const double BLACK_OFFSET = 144 - 12;

double table_width();
double table_height();
double wall_width();
double edge_width();
double ball_radius();
double cue_height();
double cue_width();
double pocket_size();
double power_bar_width();
double power_bar_height();
double semicircle_radius();

vector_t base_pos(double x_offset, double y_offset);
vector_t cue_ball_pos();
vector_t yellow_pos();
vector_t green_pos();
vector_t brown_pos();
vector_t blue_pos();
vector_t pink_pos();
vector_t black_pos();
vector_t cue_pos();
vector_t reset_button_pos();
vector_t mute_button_pos();
vector_t power_bar_pos();
vector_t slider_pos();

double triangle_wave(double amplitude, double period, double x);

bool on_table(vector_t position);
bool on_reset_button(vector_t position);
bool on_mute_button(vector_t position);

void create_edges(state_t *state);
void create_pockets(state_t *state);
void create_walls(state_t *state);
void create_triangle(state_t *state);
void create_balls(state_t *state);
void create_semicircle(state_t *state);
void create_cue(state_t *state);
void create_slider(state_t *state);
void create_reset_button(state_t *state);

bool ball_in_table(body_t *ball);
bool ball_near_table_edge(body_t *ball);
bool ball_in_semicircle(body_t *ball);
void create_dotted_lines(state_t *state, vector_t axis);
void remove_dotted_lines(state_t *state);

void cue_collision_handler(body_t *cue, body_t *ball, vector_t axis, void *aux);
void pocket_collision_handler(body_t *pocket, body_t *ball, vector_t axis,
                              void *aux);
void ball_collision_handler(body_t *body1, body_t *body2, vector_t axis,
                            void *aux);
void sound_handler(sound_set_t *sound_set, body_t *body1, body_t *body2);
void apply_forces(state_t *state);

void game_state_toggle_mute(state_t *state);

void game_state_init(state_t *state);
#endif
