#include "game_state.h"
#include "sdl_wrapper.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>

double table_width() { return TABLE_WIDTH * SCALE; }
double table_height() { return TABLE_HEIGHT * SCALE; }
double wall_width() { return WALL_WIDTH * SCALE; }
double edge_width() { return EDGE_WIDTH * SCALE; }
double ball_radius() { return BALL_RADIUS * SCALE; }
double cue_height() { return CUE_HEIGHT * SCALE; }
double cue_width() { return CUE_WIDTH * SCALE; }
double pocket_size() { return 5 * ball_radius(); }
double power_bar_width() { return POWER_BAR_WIDTH * SCALE; }
double power_bar_height() { return POWER_BAR_HEIGHT * SCALE; }
double power_marker_width() { return POWER_MARKER_WIDTH * SCALE; }
double power_marker_height() { return POWER_MARKER_HEIGHT * SCALE; }
double semicircle_radius() { return SEMICIRCLE_RADIUS * SCALE; }

vector_t base_pos(double x_offset, double y_offset) {
  return (vector_t){(MAX_POS.x - MIN_POS.x - table_height()) / 2 +
                        x_offset * SCALE,
                    (MAX_POS.y - MIN_POS.y) / 2 + y_offset * SCALE};
}
vector_t cue_ball_pos() {
  return base_pos(BALK_OFFSET - SEMICIRCLE_RADIUS / 2, 0);
}
vector_t yellow_pos() { return base_pos(BALK_OFFSET, -SEMICIRCLE_RADIUS); }
vector_t green_pos() { return base_pos(BALK_OFFSET, SEMICIRCLE_RADIUS); }
vector_t brown_pos() { return base_pos(BALK_OFFSET, 0); }
vector_t blue_pos() { return base_pos(BLUE_OFFSET, 0); }
vector_t pink_pos() { return base_pos(PINK_OFFSET, 0); }
vector_t black_pos() { return base_pos(BLACK_OFFSET, 0); }
vector_t cue_pos() {
  return (vector_t){base_pos(0, 0).x / 2, base_pos(0, 0).y};
}
vector_t reset_button_pos() {
  return (vector_t){MAX_POS.x - BUTTON_PADDING, MAX_POS.y - BUTTON_PADDING};
}
vector_t mute_button_pos() {
  return (vector_t){MAX_POS.x - BUTTON_PADDING, MAX_POS.y - 3 * BUTTON_PADDING};
}
vector_t power_bar_pos() {
  return (vector_t){MAX_POS.x - base_pos(0, 0).x + power_bar_width(),
                    base_pos(0, 0).y - power_bar_height() / 2};
}
vector_t slider_pos() {
  return (vector_t){power_bar_pos().x + (power_bar_width() / 2),
                    power_bar_pos().y +
                        power_bar_height() * POWER_SLIDER_OFFSET};
}

double triangle_wave(double amplitude, double period, double x) {
  return amplitude * (1 - fabs(fmod(x, 2 * period) - period) / period);
}

bool on_table(vector_t position) {
  double x_center = (MAX_POS.x - MIN_POS.x) / 2;
  double y_center = (MAX_POS.y - MIN_POS.y) / 2;
  double x_min = x_center - table_height() / 2;
  double x_max = x_center + table_height() / 2;
  double y_min = y_center - table_width() / 2;
  double y_max = y_center + table_width() / 2;
  return (position.x >= x_min && position.x <= x_max && position.y >= y_min &&
          position.y <= y_max);
}

bool on_reset_button(vector_t position) {
  return (vec_magnitude(vec_subtract(position, (vector_t)reset_button_pos())) <=
          BUTTON_RADIUS);
}

bool on_mute_button(vector_t position) {
  return (vec_magnitude(vec_subtract(position, (vector_t)mute_button_pos())) <=
          BUTTON_RADIUS);
}

void create_edges(state_t *state) {
  double x_center = (MAX_POS.x - MIN_POS.x) / 2;
  double y_center = (MAX_POS.y - MIN_POS.y) / 2;
  double x_base = x_center - table_height() / 2;
  double y_base = y_center - table_width() / 2;

  int *info = malloc(sizeof(int));
  *info = WALL_INFO;

  vector_t p1 = {x_center + pocket_size() / 2, y_base};
  vector_t p2 = {x_center + pocket_size() / 2 + edge_width() / 2,
                 y_base + edge_width()};
  vector_t p3 = {x_center + table_height() / 2 - pocket_size() / 2 * sqrt(2) -
                     edge_width(),
                 y_base + edge_width()};
  vector_t p4 = {x_center + table_height() / 2 - pocket_size() / 2 * sqrt(2),
                 y_base};
  list_t *shape_x1 = draw_quadrilateral(p1, p2, p3, p4);
  body_t *wall_x1 =
      body_init_with_info(shape_x1, INFINITY, DARK_GRAY, info, free);
  list_t *shape_x2 = body_get_shape(wall_x1);
  list_t *shape_x3 = body_get_shape(wall_x1);
  list_t *shape_x4 = body_get_shape(wall_x1);
  polygon_reflect_x(shape_x2, x_center);
  polygon_reflect_y(shape_x3, y_center);
  polygon_reflect_x(shape_x4, x_center);
  polygon_reflect_y(shape_x4, y_center);
  body_t *wall_x2 =
      body_init_with_info(shape_x2, INFINITY, DARK_GRAY, info, NULL);
  body_t *wall_x3 =
      body_init_with_info(shape_x3, INFINITY, DARK_GRAY, info, NULL);
  body_t *wall_x4 =
      body_init_with_info(shape_x4, INFINITY, DARK_GRAY, info, NULL);

  p1 = (vector_t){x_base, y_base + pocket_size() / 2 * sqrt(2)};
  p2 = (vector_t){x_base + edge_width(),
                  y_base + pocket_size() / 2 * sqrt(2) + edge_width()};
  p3 = (vector_t){x_base + edge_width(), y_center + table_width() / 2 -
                                             pocket_size() / 2 * sqrt(2) -
                                             edge_width()};
  p4 = (vector_t){x_base,
                  y_center + table_width() / 2 - pocket_size() / 2 * sqrt(2)};
  list_t *shape_y1 = draw_quadrilateral(p1, p2, p3, p4);
  body_t *wall_y1 =
      body_init_with_info(shape_y1, INFINITY, DARK_GRAY, info, NULL);
  list_t *shape_y2 = body_get_shape(wall_y1);
  polygon_reflect_x(shape_y2, x_center);
  body_t *wall_y2 =
      body_init_with_info(shape_y2, INFINITY, DARK_GRAY, info, NULL);
  bool hidden = true;
  body_hide(wall_x1, hidden);
  body_hide(wall_x2, hidden);
  body_hide(wall_x3, hidden);
  body_hide(wall_x4, hidden);
  body_hide(wall_y1, hidden);
  body_hide(wall_y2, hidden);

  scene_add_body(state->scene, wall_x1);
  scene_add_body(state->scene, wall_x2);
  scene_add_body(state->scene, wall_x3);
  scene_add_body(state->scene, wall_x4);
  scene_add_body(state->scene, wall_y1);
  scene_add_body(state->scene, wall_y2);
}

void create_pockets(state_t *state) {
  double x_center = (MAX_POS.x - MIN_POS.x) / 2;
  double y_center = (MAX_POS.y - MIN_POS.y) / 2;
  double x_base = x_center - table_height() / 2;
  double y_base = y_center - table_width() / 2;
  double depth = pocket_size();
  list_t *pockets = list_init(18, NULL);

  int *info = malloc(sizeof(int));
  *info = POCKET_INFO;

  vector_t s_p1 = {x_center + pocket_size() / 2, y_base - ball_radius() * 4.25};
  vector_t s_p2 = {x_center - pocket_size() / 2, y_base - ball_radius() * 4.25};
  vector_t s_p3 = {s_p2.x, s_p2.y + ball_radius() * 2};
  vector_t s_p4 = {s_p1.x, s_p1.y + ball_radius() * 2};
  vector_t s_p5 = {s_p1.x, y_base};
  vector_t s_p6 = {s_p5.x - pocket_size() / 6, s_p4.y};

  list_t *shape_s1 = draw_quadrilateral(s_p1, s_p2, s_p3, s_p4);
  list_t *shape_s2 = polygon_copy(shape_s1);
  polygon_reflect_y(shape_s2, y_center);

  list_t *shape_s11 = draw_triangle(s_p4, s_p5, s_p6);
  list_t *shape_s12 = polygon_copy(shape_s11);
  list_t *shape_s21 = polygon_copy(shape_s11);
  list_t *shape_s22 = polygon_copy(shape_s11);
  polygon_reflect_x(shape_s12, x_center);
  polygon_reflect_y(shape_s21, y_center);
  polygon_reflect_x(shape_s22, x_center);
  polygon_reflect_y(shape_s22, y_center);

  vector_t c_p1 = {x_base - 0.5 * depth / sqrt(2),
                   y_base + 0.5 * depth / sqrt(2)};
  vector_t c_p2 = {x_base + 0.5 * depth / sqrt(2),
                   y_base - 0.5 * depth / sqrt(2)};
  vector_t c_p3 = {c_p1.x, c_p2.y};
  vector_t c_p4 = {x_base, y_base + pocket_size() / 2 * sqrt(2)};
  vector_t c_p5 = {c_p1.x, c_p4.y};
  vector_t c_p6 = {x_base + pocket_size() / 2 * sqrt(2), y_base};
  vector_t c_p7 = {c_p6.x, c_p2.y};

  list_t *shape_c1 = draw_triangle(c_p1, c_p2, c_p3);
  list_t *shape_c2 = polygon_copy(shape_c1);
  list_t *shape_c3 = polygon_copy(shape_c1);
  list_t *shape_c4 = polygon_copy(shape_c1);
  polygon_reflect_x(shape_c2, x_center);
  polygon_reflect_y(shape_c3, y_center);
  polygon_reflect_x(shape_c4, x_center);
  polygon_reflect_y(shape_c4, y_center);

  list_t *shape_c11 = draw_triangle(c_p1, c_p4, c_p5);
  list_t *shape_c12 = draw_triangle(c_p2, c_p6, c_p7);
  list_t *shape_c21 = polygon_copy(shape_c11);
  list_t *shape_c22 = polygon_copy(shape_c12);
  list_t *shape_c31 = polygon_copy(shape_c11);
  list_t *shape_c32 = polygon_copy(shape_c12);
  list_t *shape_c41 = polygon_copy(shape_c11);
  list_t *shape_c42 = polygon_copy(shape_c12);
  polygon_reflect_x(shape_c21, x_center);
  polygon_reflect_x(shape_c22, x_center);
  polygon_reflect_y(shape_c31, y_center);
  polygon_reflect_y(shape_c32, y_center);
  polygon_reflect_x(shape_c41, x_center);
  polygon_reflect_x(shape_c42, x_center);
  polygon_reflect_y(shape_c41, y_center);
  polygon_reflect_y(shape_c42, y_center);

  list_add(pockets, shape_s1);
  list_add(pockets, shape_s11);
  list_add(pockets, shape_s12);
  list_add(pockets, shape_s2);
  list_add(pockets, shape_s21);
  list_add(pockets, shape_s22);
  list_add(pockets, shape_c1);
  list_add(pockets, shape_c11);
  list_add(pockets, shape_c12);
  list_add(pockets, shape_c2);
  list_add(pockets, shape_c21);
  list_add(pockets, shape_c22);
  list_add(pockets, shape_c3);
  list_add(pockets, shape_c31);
  list_add(pockets, shape_c32);
  list_add(pockets, shape_c4);
  list_add(pockets, shape_c41);
  list_add(pockets, shape_c42);

  bool hidden = true;
  for (int i = 0; i < list_size(pockets); i++) {
    list_t *curr = list_get(pockets, i);
    body_t *pocket;
    if (i) {
      pocket = body_init_with_info(curr, INFINITY, DARK_GRAY, info, NULL);
    } else {
      pocket = body_init_with_info(curr, INFINITY, DARK_GRAY, info, free);
    }
    body_hide(pocket, hidden);
    scene_add_body(state->scene, pocket);
  }
  list_free(pockets);
}

void create_walls(state_t *state) {
  vector_t centroid_1 = {
      (MAX_POS.x - MIN_POS.x) / 2,
      (MAX_POS.y - MIN_POS.y - table_width() - wall_width()) / 2};
  vector_t centroid_2 = {
      (MAX_POS.x - MIN_POS.x) / 2,
      (MAX_POS.y - MIN_POS.y + table_width() + wall_width()) / 2};
  vector_t centroid_3 = {
      (MAX_POS.x - MIN_POS.x - table_height() - wall_width()) / 2,
      (MAX_POS.y - MIN_POS.y) / 2};
  vector_t centroid_4 = {
      (MAX_POS.x - MIN_POS.x + table_height() + wall_width()) / 2,
      (MAX_POS.y - MIN_POS.y) / 2};
  int *info = malloc(sizeof(int));
  *info = WALL_INFO;
  body_t *wall1 = body_init_with_info(
      draw_rectangle(&centroid_1, table_height(), wall_width()), INFINITY,
      BLACK, info, free);
  body_t *wall2 = body_init_with_info(
      draw_rectangle(&centroid_2, table_height(), wall_width()), INFINITY,
      BLACK, info, NULL);
  body_t *wall3 = body_init_with_info(
      draw_rectangle(&centroid_3, wall_width(), table_width()), INFINITY, BLACK,
      info, NULL);
  body_t *wall4 = body_init_with_info(
      draw_rectangle(&centroid_4, wall_width(), table_width()), INFINITY, BLACK,
      info, NULL);
  scene_add_body(state->scene, wall1);
  scene_add_body(state->scene, wall2);
  scene_add_body(state->scene, wall3);
  scene_add_body(state->scene, wall4);
}

void create_triangle(state_t *state) {
  double radius = ball_radius() * 1.1;
  vector_t initial_centroid = pink_pos();
  initial_centroid.x += 3 * ball_radius();
  for (int i = 0; i < 5; i++) {
    vector_t centroid = initial_centroid;
    for (int j = 0; j <= i; j++) {
      info_t *info = malloc(sizeof(info_t));
      *info = RED_INFO;
      char *image_path = "assets/Red.png";
      SDL_Texture *image = sdl_load_image(image_path);
      body_t *ball =
          body_init_with_info_and_sprite(draw_circle(&centroid, ball_radius()),
                                         BALL_MASS, RED, info, image, free);
      body_set_dimensions(ball,
                          (vector_t){2 * ball_radius(), 2 * ball_radius()});
      body_set_shadow(ball, sdl_load_image("assets/Shadow.png"));
      scene_add_body(state->scene, ball);
      centroid.y += 2 * radius;
    }
    initial_centroid.x += radius * sqrt(3);
    initial_centroid.y -= radius;
  }
}

void create_balls(state_t *state) {
  create_triangle(state);
  for (int i = 0; i < 7; i++) {
    vector_t *centroid = malloc(sizeof(vector_t));
    info_t *info = malloc(sizeof(info));
    char *image_path = malloc(sizeof(char *));
    rgb_color_t color = GRAY;
    switch (i) {
    case 0:
      *centroid = cue_ball_pos();
      *info = CUE_BALL_INFO;
      image_path = "assets/White.png";
      break;
    case 1:
      *centroid = yellow_pos();
      *info = YELLOW_INFO;
      color = YELLOW;
      image_path = "assets/Yellow.png";
      break;
    case 2:
      *centroid = green_pos();
      *info = GREEN_INFO;
      color = GREEN;
      image_path = "assets/Green.png";
      break;
    case 3:
      *centroid = brown_pos();
      *info = BROWN_INFO;
      color = BROWN;
      image_path = "assets/Brown.png";
      break;
    case 4:
      *centroid = blue_pos();
      *info = BLUE_INFO;
      color = BLUE;
      image_path = "assets/Blue.png";
      break;
    case 5:
      *centroid = pink_pos();
      *info = PINK_INFO;
      color = PINK;
      image_path = "assets/Pink.png";
      break;
    case 6:
      *centroid = black_pos();
      *info = BLACK_INFO;
      color = BLACK;
      image_path = "assets/Black.png";
      break;
    }
    list_t *points = draw_circle(centroid, ball_radius());
    SDL_Texture *image = sdl_load_image(image_path);
    body_t *ball = body_init_with_info_and_sprite(points, BALL_MASS, color,
                                                  info, image, free);
    body_set_dimensions(ball, (vector_t){2 * ball_radius(), 2 * ball_radius()});
    body_set_shadow(ball, sdl_load_image("assets/Shadow.png"));
    body_set_respawnable(ball, true);
    if (i == CUE_BALL_INFO) {
      state->cue_ball = ball;
    } else {
      scene_add_body(state->scene, ball);
    }
    free(centroid);
  }
  scene_add_body(state->scene, state->cue_ball);
}

void create_semicircle(state_t *state) {
  vector_t *semicircle_centroid = malloc(sizeof(vector_t));
  *semicircle_centroid = base_pos(BALK_OFFSET, 0);
  state->semicircle = draw_arc(semicircle_centroid, semicircle_radius(),
                               M_PI / 2, 3 * M_PI / 2);
  // body_t *sc = body_init(state->semicircle, 1, LIGHT_GRAY);
  // scene_add_body(state->scene, sc);
}

void create_cue(state_t *state) {
  vector_t centroid = cue_pos();
  double width = CUE_WIDTH * SCALE;
  vector_t p1 = {centroid.x - width / 2, centroid.y - cue_height() / 2};
  vector_t p2 = {centroid.x + width / 2, centroid.y - cue_height() / 2};
  vector_t p3 = {centroid.x + width / 2, centroid.y + cue_height() / 2};
  vector_t p4 = {centroid.x - width / 2, centroid.y + cue_height() / 2};
  list_t *shape = draw_quadrilateral(p1, p2, p3, p4);
  info_t *info = malloc(sizeof(info));
  *info = CUE_INFO;
  char *image_path = "assets/CueWood.png";
  SDL_Texture *image = sdl_load_image(image_path);
  state->cue = body_init_with_info_and_sprite(shape, TABLE_MASS, MAGENTA, info,
                                              image, free);
  body_set_dimensions(state->cue, (vector_t){cue_width(), cue_height()});
  scene_add_body(state->scene, state->cue);
}

void create_table(state_t *state) {
  vector_t centroid =
      (vector_t){(MAX_POS.x - MIN_POS.x) / 2, (MAX_POS.y - MIN_POS.y) / 2};
  vector_t p1 = {centroid.x - table_height() / 2,
                 centroid.y - table_width() / 2};
  vector_t p2 = {centroid.x - table_height() / 2,
                 centroid.y + table_width() / 2};
  vector_t p3 = {centroid.x + table_height() / 2,
                 centroid.y + table_width() / 2};
  vector_t p4 = {centroid.x + table_height() / 2,
                 centroid.y - table_width() / 2};
  list_t *shape = draw_quadrilateral(p1, p2, p3, p4);
  char *image_path = "assets/TableLowerdpi.png";
  SDL_Texture *image = sdl_load_image(image_path);
  body_t *table =
      body_init_with_info_and_sprite(shape, CUE_MASS, WHITE, NULL, image, free);
  body_set_dimensions(table, (vector_t){table_height() + 8 * edge_width(),
                                        table_width() + 8 * edge_width()});
  // body_hide(table, true);
  scene_add_body(state->scene, table);
  state->table = shape;
}

void create_powerbar(state_t *state) {
  char *image_path = "assets/PowerBar.png";
  SDL_Texture *image = sdl_load_image(image_path);
  vector_t centroid = (vector_t){power_bar_pos().x + power_bar_width() / 2,
                                 power_bar_pos().y + power_bar_height() / 2};
  list_t *shape =
      draw_rectangle(&centroid, power_bar_width(), power_bar_height());
  body_t *power_bar =
      body_init_with_info_and_sprite(shape, CUE_MASS, WHITE, NULL, image, free);
  body_set_dimensions(power_bar,
                      (vector_t){power_bar_width(), power_bar_height()});
  scene_add_body(state->scene, power_bar);
}

void create_floor(state_t *state) {
  vector_t p1 = MIN_POS;
  vector_t p2 = {MIN_POS.x, MAX_POS.y};
  vector_t p3 = MAX_POS;
  vector_t p4 = {MAX_POS.x, MIN_POS.y};
  list_t *shape = draw_quadrilateral(p1, p2, p3, p4);
  info_t *info = malloc(sizeof(info));
  char *image_path = "assets/Floor.png";
  SDL_Texture *image = sdl_load_image(image_path);
  body_t *floor = body_init_with_info_and_sprite(shape, CUE_MASS, MAGENTA, NULL,
                                                 image, free);
  body_set_dimensions(floor, MAX_POS);
  // body_hide(floor, true);
  scene_add_body(state->scene, floor);
}

void create_slider(state_t *state) {
  vector_t centroid = slider_pos();
  list_t *shape = draw_rectangle(&centroid, 100, 100);
  char *image_path = "assets/PowerMarker.png";
  SDL_Texture *image = sdl_load_image(image_path);
  state->slider =
      body_init_with_info_and_sprite(shape, INFINITY, BLACK, NULL, image, free);
  body_set_dimensions(state->slider,
                      (vector_t){power_marker_width(), power_marker_height()});
  body_hide(state->slider, true);
  scene_add_body(state->scene, state->slider);
}

void create_reset_button(state_t *state) {
  vector_t centroid = reset_button_pos();
  list_t *shape = draw_circle(&centroid, BUTTON_RADIUS);
  char *image_path = "assets/ResetButton.png";
  SDL_Texture *image = sdl_load_image(image_path);
  state->reset_button =
      body_init_with_info_and_sprite(shape, INFINITY, GRAY, NULL, image, free);
  scene_add_body(state->scene, state->reset_button);
}

void create_mute_button(state_t *state) {
  vector_t centroid = mute_button_pos();
  list_t *shape = draw_circle(&centroid, BUTTON_RADIUS);
  char *image_path = "assets/MuteButton.png";
  SDL_Texture *image = sdl_load_image(image_path);
  state->mute_button = body_init_with_info_and_sprite(shape, INFINITY, MAGENTA,
                                                      NULL, image, free);
  scene_add_body(state->scene, state->mute_button);
}

bool ball_in_table(body_t *ball) {
  double x_center = (MAX_POS.x - MIN_POS.x) / 2;
  double y_center = (MAX_POS.y - MIN_POS.y) / 2;
  vector_t centroid = body_get_centroid(ball);
  double x_min =
      x_center - table_height() / 2 + ball_radius() + edge_width() + 1;
  double x_max =
      x_center + table_height() / 2 - ball_radius() - edge_width() - 1;
  double y_min =
      y_center - table_width() / 2 + ball_radius() + edge_width() + 1;
  double y_max =
      y_center + table_width() / 2 - ball_radius() - edge_width() - 1;

  if (centroid.x > x_max) {
    body_set_centroid(ball, (vector_t){x_max, centroid.y});
    return false;
  } else if (centroid.x < x_min) {
    body_set_centroid(ball, (vector_t){x_min, centroid.y});
    return false;
  } else if (centroid.y > y_max) {
    body_set_centroid(ball, (vector_t){centroid.x, y_max});
    return false;
  } else if (centroid.y < y_min) {
    body_set_centroid(ball, (vector_t){centroid.x, y_min});
    return false;
  }
  return true;
}

bool ball_near_table_edge(body_t *ball) {
  double x_center = (MAX_POS.x - MIN_POS.x) / 2;
  double y_center = (MAX_POS.y - MIN_POS.y) / 2;
  vector_t centroid = body_get_centroid(ball);
  if (fabs(centroid.x - x_center) < 0.475 * table_height() &&
      fabs(centroid.y - y_center) < 0.45 * table_width()) {
    return false;
  }
  double x_min =
      x_center - table_height() / 2 + ball_radius() + edge_width() - 1;
  double x_max =
      x_center + table_height() / 2 - ball_radius() - edge_width() + 1;
  double y_min =
      y_center - table_width() / 2 + ball_radius() + edge_width() - 1;
  double y_max =
      y_center + table_width() / 2 - ball_radius() - edge_width() + 1;

  if (centroid.x > x_max) {
    body_set_centroid(ball, (vector_t){x_max, centroid.y});
  } else if (centroid.x < x_min) {
    body_set_centroid(ball, (vector_t){x_min, centroid.y});
  } else if (centroid.y > y_max) {
    body_set_centroid(ball, (vector_t){centroid.x, y_max});
  } else if (centroid.y < y_min) {
    body_set_centroid(ball, (vector_t){centroid.x, y_min});
  }
  return true;
}

bool ball_in_semicircle(body_t *ball) {
  vector_t centroid = body_get_centroid(ball);
  vector_t circle_center = brown_pos();
  if (centroid.x > circle_center.x) {
    body_set_centroid(ball, (vector_t){circle_center.x, centroid.y});
    return false;
  }
  vector_t difference = vec_subtract(centroid, circle_center);
  double radius = semicircle_radius();
  if (vec_magnitude(difference) <= radius) {
    return true;
  }
  vector_t new_centroid =
      vec_add(circle_center, vec_init(radius - 1, vec_direction(difference)));
  body_set_centroid(ball, new_centroid);
  return false;
}

void create_dotted_lines(state_t *state, vector_t axis) {
  if (!state->training_lines) {
    return;
  }

  int separation = 150;
  int increment = 20;
  list_t *lines = list_init(10, NULL);
  vector_t initial_c = body_get_centroid(state->cue_ball);
  body_t *ball =
      body_init(draw_circle(&initial_c, ball_radius()), BALL_MASS, WHITE);
  vector_t velocity = vec_init(increment, vec_direction(axis));
  int steps = 0;
  int collisions = 0;
  while (collisions < MAX_COLLISIONS) {
    body_translate(ball, velocity);
    for (int i = 0; i < scene_bodies(state->scene); i++) {
      body_t *curr = scene_get_body(state->scene, i);
      int *info = body_get_info(curr);
      if (info == NULL || *info == CUE_BALL_INFO || *info == CUE_INFO) {
        continue;
      }
      list_t *shape1 = body_get_shape(ball);
      list_t *shape2 = body_get_shape(curr);
      collision_info_t collision = find_collision(shape1, shape2);
      list_free(shape1);
      list_free(shape2);
      if (collision.collided) {
        if (*info == WALL_INFO) {
          ball_near_table_edge(ball);
        } else {
          collisions = MAX_COLLISIONS;
          break;
        }
        vector_t impulse = vec_multiply(-(1 + B_W_ELASTICITY) *
                                            vec_dot(velocity, collision.axis),
                                        collision.axis);
        velocity = vec_add(velocity, impulse);
        collisions++;
        break;
      }
    }
    vector_t c = body_get_centroid(ball);
    if (collisions == MAX_COLLISIONS) {
      list_t *shape = draw_circle(&c, ball_radius() * 2 / 3);
      body_t *end = body_init(shape, INFINITY, WHITE);
      scene_add_body(state->scene, end);
      list_add(lines, end);
    } else if (steps % (separation / increment) == separation / increment / 2) {
      list_t *shape = draw_rectangle(&c, LINE_LENGTH, LINE_WIDTH);
      body_t *line = body_init(shape, INFINITY, WHITE);
      body_rotate(line, vec_direction(velocity));
      scene_add_body(state->scene, line);
      list_add(lines, line);
    }
    steps++;
  }
  state->dotted_lines = lines;
}

void remove_dotted_lines(state_t *state) {
  if (state->dotted_lines == NULL) {
    return;
  }
  for (int i = 0; i < list_size(state->dotted_lines); i++) {
    body_remove((body_t *)list_get(state->dotted_lines, i));
  }
  list_free(state->dotted_lines);
  state->dotted_lines = NULL;
}

void cue_collision_handler(body_t *cue, body_t *ball, vector_t axis,
                           void *aux) {
  state_t *state = aux;
  double chalk = state->chalk[state->player];
  double elasticity = CUE_ELASTICITY * chalk;
  state->chalk[state->player] =
      chalk < MINIMUM_CHALK ? chalk : chalk - CHALK_DECAY_RATE;
  vector_t v1 = body_get_velocity(cue);
  vector_t v2 = body_get_velocity(ball);
  double reduced_mass = 0;
  if (isinf(body_get_mass(cue))) {
    reduced_mass = body_get_mass(ball);
  } else if (isinf(body_get_mass(ball))) {
    reduced_mass = body_get_mass(cue);
  } else {
    reduced_mass = body_get_mass(cue) * body_get_mass(ball) /
                   (body_get_mass(cue) + body_get_mass(ball));
  }
  double J =
      reduced_mass * (1 + elasticity) * vec_dot(vec_subtract(v2, v1), axis);
  vector_t impulse = vec_multiply(-J, axis);
  body_add_impulse(ball, impulse);
  body_set_to_respawn(cue, true);
  body_set_apply_forces(cue, false);
}

void pocket_collision_handler(body_t *pocket, body_t *ball, vector_t axis,
                              void *aux) {
  state_t *state = aux;
  if (body_get_respawnable(ball)) {
    body_set_to_respawn(ball, true);
  } else {
    body_remove(ball);
  }

  int info = *(int *)body_get_info(ball);
  if (info == CUE_BALL_INFO) {
    int foul = fmax(state->ball_on, 4);
    state->foul = fmax(state->foul, foul);
  } else if (info == RED_INFO) {
    if (state->ball_on == 1) {
      state->points++;
    } else {
      state->foul = fmax(state->foul, 4);
    }
  } else {
    if (state->reds_left) {
      if (!state->points && state->ball_on != 1) {
        state->points = info;
      } else {
        int foul = fmax(info, 4);
        state->foul = fmax(state->foul, foul);
      }
    } else if (info == state->ball_on) {
      state->points = info;
    } else {
      int foul = fmax(info, 4);
      state->foul = fmax(state->foul, foul);
    }
  }
}

void ball_collision_handler(body_t *body1, body_t *body2, vector_t axis,
                            void *aux) {
  state_t *state = aux;
  vector_t v1 = body_get_velocity(body1);
  vector_t v2 = body_get_velocity(body2);
  double elasticity = B_B_ELASTICITY;
  double reduced_mass = 0;
  if (isinf(body_get_mass(body1))) {
    reduced_mass = body_get_mass(body2);
  } else if (isinf(body_get_mass(body2))) {
    reduced_mass = body_get_mass(body1);
  } else {
    reduced_mass = body_get_mass(body1) * body_get_mass(body2) /
                   (body_get_mass(body1) + body_get_mass(body2));
  }
  double J =
      reduced_mass * (1 + elasticity) * vec_dot(vec_subtract(v2, v1), axis);
  vector_t impulse = vec_multiply(J, axis);
  body_add_impulse(body1, impulse);
  body_add_impulse(body2, vec_negate(impulse));

  int info1 = *(int *)body_get_info(body1);
  int info2 = *(int *)body_get_info(body2);

  if (info2 == CUE_BALL_INFO) {
    info2 = info1;
    info1 = CUE_BALL_INFO;
  }
  if (info1 == CUE_BALL_INFO && state->first_hit) {
    if (!state->reds_left && info2 != state->ball_on) {
      state->foul = fmax(info2, 4);
    } else if (state->ball_on == 1 && info2 > RED_INFO) {
      state->foul = fmax(info2, 4);
    } else if (state->ball_on != 1 && info2 == RED_INFO) {
      state->foul = 4;
    }
    state->first_hit = false;
  }
}

void sound_handler(sound_set_t *sound_set, body_t *body1, body_t *body2) {
  if (!sound_set_get_muted(sound_set)) {
    int *info1 = body_get_info(body1);
    int *info2 = body_get_info(body2);
    if (*info1 == CUE_INFO || *info2 == CUE_INFO) {
      Mix_PlayChannel(-1, get_cue_ball(sound_set), 0);
    } else if (*info1 == WALL_INFO || *info2 == WALL_INFO) {
      Mix_PlayChannel(-1, get_wall_ball(sound_set), 0);
    } else if (*info1 == POCKET_INFO || *info2 == POCKET_INFO) {
      Mix_PlayChannel(-1, get_pocket_ball(sound_set), 0);
    } else {
      Mix_PlayChannel(-1, get_ball_ball(sound_set), 0);
    }
  }
}

void apply_forces(state_t *state) {
  size_t body_count = scene_bodies(state->scene);
  for (size_t i = 0; i < body_count; i++) {
    body_t *body1 = scene_get_body(state->scene, i);
    int *info1 = body_get_info(body1);
    if (*info1 <= BLACK_INFO) {
      create_gravity_friction(state->scene, MU * G, body1);
    }

    for (size_t j = i + 1; j < body_count; j++) {
      body_t *body2 = scene_get_body(state->scene, j);
      int *info2 = body_get_info(body2);

      if (*info1 == CUE_BALL_INFO &&
          *info2 == CUE_INFO) { // create physics for cue (only should interact
                                // with cue ball)
        create_collision_with_sound(state->scene, state->cue, state->cue_ball,
                                    cue_collision_handler, sound_handler, state,
                                    NULL);
      } else if (*info1 <= BLACK_INFO) { // create physics for balls
        if (*info2 <= BLACK_INFO) {
          create_collision_with_sound(state->scene, body1, body2,
                                      ball_collision_handler, sound_handler,
                                      state, NULL);
        }
      } else if (*info1 == WALL_INFO) { // create physics for walls
        if (*info2 <= BLACK_INFO) {
          create_physics_collision_with_sound(state->scene, B_W_ELASTICITY,
                                              body1, body2, sound_handler);
        }
      } else if (*info1 == POCKET_INFO) { // create physics for pockets
        if (*info2 <= BLACK_INFO) {
          create_collision_with_sound(state->scene, body1, body2,
                                      pocket_collision_handler, sound_handler,
                                      state, NULL);
        }
      }
    }
  }
}

void game_state_toggle_mute(state_t *state) {
  char *image_path = "assets/MuteButton.png";
  if (sound_set_get_muted(scene_get_sound_set(state->scene))) {
    image_path = "assets/MuteButtonON.png";
  }
  SDL_Texture *image = sdl_load_image(image_path);
  body_set_image(state->mute_button, image);
}

void game_state_init(state_t *state) {
  state->scene = scene_init_with_audio("assets/BackgroundJazz_Quiet.wav");
  scene_add_sound_set(state->scene, "assets/BallBallCollision-[CROPPED_2].wav",
                      "assets/CueBallCollision-[CROPPED_2].wav",
                      "assets/PocketBallCollision-[CROPPED_2].wav",
                      "assets/WallBallCollision-[CROPPED_2].wav");
  state->goto_next_state = false;
  state->flags = SET_CUE_BALL | CUE_HIT;
  state->in_alt_state = false;
  state->dotted_lines = NULL;
  state->player = 0;
  state->scores[0] = 0;
  state->scores[1] = 0;
  state->foul = 0;
  state->points = 0;
  state->ball_on = 1;
  state->time = 0;
  state->first_hit = true;
  state->reds_left = true;
  state->training_lines = true;
  state->chalk[0] = 1;
  state->chalk[1] = 1;
  create_semicircle(state);
  create_floor(state);
  create_table(state);
  create_edges(state);
  create_pockets(state);
  create_balls(state);
  create_reset_button(state);
  create_mute_button(state);
  create_powerbar(state);
  create_slider(state);
  create_cue(state);
  apply_forces(state);
}