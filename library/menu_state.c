#include "menu_state.h"
#include "game_state.h"

void create_rules_page(state_t *state) {
  vector_t p1 = MIN_POS;
  vector_t p2 = {MIN_POS.x, MAX_POS.y};
  vector_t p3 = MAX_POS;
  vector_t p4 = {MAX_POS.x, MIN_POS.y};
  list_t *shape = draw_quadrilateral(p1, p2, p3, p4);
  info_t *info = malloc(sizeof(info));
  char *image_path = "assets/RulesDemo.png"; // this is the path to the rules
                                             // image (placeholder).
  // i'll load an image with the rules and text once we have the rules
  // incorporated in gameplay so i know which ones to actually write down.
  SDL_Texture *image = sdl_load_image(image_path);
  body_t *page = body_init_with_info_and_sprite(shape, CUE_MASS, MAGENTA, NULL,
                                                image, free);
  scene_add_body(state->scene, page);
}

void state_goto_next_state(state_t *state) { state->goto_next_state = true; }

void state_goto_alt_state(state_t *state) {
  state->in_alt_state = true;
  for (int i = 0; i < scene_bodies(state->scene); i++) {
    scene_remove_body(state->scene, i);
  }
  create_rules_page(state);
  create_reset_button(state);
}

vector_t start_button_pos() {
  return (vector_t){(MAX_POS.x - MIN_POS.x) / 2, (MAX_POS.y - MIN_POS.y) / 2};
}

vector_t rules_button_pos() {
  return vec_add((vector_t){0, (MIN_POS.y - MAX_POS.y / 5)},
                 start_button_pos());
}

bool on_rectangular_button(vector_t position, vector_t button_pos) {
  return (position.x >= button_pos.x - BUTTON_WIDTH / 2 &&
          position.x <= button_pos.x + BUTTON_WIDTH / 2 &&
          position.y >= button_pos.y - BUTTON_HEIGHT / 2 &&
          position.y <= button_pos.y + BUTTON_HEIGHT / 2);
}

bool on_start_button(vector_t position) {
  return on_rectangular_button(position, start_button_pos());
}

bool on_rules_button(vector_t position) {
  return on_rectangular_button(position, rules_button_pos());
}

void create_start_button(state_t *state) {
  vector_t centroid = (vector_t)start_button_pos();
  list_t *shape = draw_rectangle(&centroid, BUTTON_WIDTH, BUTTON_HEIGHT);
  char *image_path = "assets/GameButton.png";
  SDL_Texture *image = sdl_load_image(image_path);
  state->start_button =
      body_init_with_info_and_sprite(shape, INFINITY, GREEN, NULL, image, free);
  body_set_dimensions(state->start_button,
                      (vector_t){BUTTON_WIDTH, BUTTON_HEIGHT});
  scene_add_body(state->scene, state->start_button);
}

void create_rules_button(state_t *state) {
  vector_t centroid = (vector_t)rules_button_pos();
  list_t *shape = draw_rectangle(&centroid, BUTTON_WIDTH, BUTTON_HEIGHT);
  char *image_path = "assets/RulesButton.png";
  SDL_Texture *image = sdl_load_image(image_path);
  state->rules_button =
      body_init_with_info_and_sprite(shape, INFINITY, BROWN, NULL, image, free);
  body_set_dimensions(state->rules_button,
                      (vector_t){BUTTON_WIDTH, BUTTON_HEIGHT});
  scene_add_body(state->scene, state->rules_button);
}

void create_background(state_t *state) {
  vector_t p1 = MIN_POS;
  vector_t p2 = {MIN_POS.x, MAX_POS.y};
  vector_t p3 = MAX_POS;
  vector_t p4 = {MAX_POS.x, MIN_POS.y};
  list_t *shape = draw_quadrilateral(p1, p2, p3, p4);
  info_t *info = malloc(sizeof(info));
  char *image_path = "assets/MenuFloor.png";
  SDL_Texture *image = sdl_load_image(image_path);
  body_t *floor = body_init_with_info_and_sprite(shape, CUE_MASS, MAGENTA, NULL,
                                                 image, free);
  body_set_dimensions(floor, MAX_POS);
  // body_hide(floor, true);
  scene_add_body(state->scene, floor);
}

state_t *menu_state_init() {
  state_t *state = malloc(sizeof(state_t));
  sdl_init_with_title(TITLE, MIN_POS, MAX_POS);
  state->scene = scene_init();
  state->goto_next_state = false;
  state->in_alt_state = false;
  create_background(state);
  create_start_button(state);
  create_rules_button(state);
  return state;
}