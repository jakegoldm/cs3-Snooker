#ifndef __MENU_STATE_H__
#define __MENU_STATE_H__

/**
 * Header file defining everthing necessary for the menu at the beginning
 * of the game.
 */

#include "forces.h"
#include "game_state.h"
#include "polygon.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "shape.h"
#include "state.h"

static const double BUTTON_HEIGHT = 250;
static const double BUTTON_WIDTH = 1000;

void create_rules(state_t *state);

void state_goto_next_state(state_t *state);

void state_goto_alt_state(state_t *state);

vector_t start_button_pos();

vector_t rules_button_pos();

bool on_start_button(vector_t position);

bool on_rules_button(vector_t position);

void create_start_button(state_t *state);

void create_rules_button(state_t *state);

state_t *menu_state_init();

#endif