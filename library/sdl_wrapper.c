#include "sdl_wrapper.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

const char WINDOW_TITLE[] = "CS 3";
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 600;
const double MS_PER_S = 1e3;
const double DEFAULT_IMG_SCALE = .6;
const double DEFAULT_SHADOW_SCALE = 1.4;

/**
 * The coordinate at the center of the screen.
 */
vector_t center;
/**
 * The coordinate difference from the center to the top right corner.
 */
vector_t max_diff;
/**
 * The SDL window where the scene is rendered.
 */
SDL_Window *window;
/**
 * The renderer used to draw the scene.
 */
SDL_Renderer *renderer;
/**
 * The keypress handler, or NULL if none has been configured.
 */
key_handler_t key_handler = NULL;
/**
 * The mouse click handler, or NULL if none has been configured.
 */
mouse_handler_t mouse_handler = NULL;
/**
 * SDL's timestamp when a key was last pressed or released.
 * Used to mesasure how long a key has been held.
 */
uint32_t key_start_timestamp;
/**
 * SDL's timestamp when a mouse was last pressed or released.
 * Used to mesasure how long a click has been held.
 */
uint32_t click_start_timestamp = 0;
/**
 * The value of clock() when time_since_last_tick() was last called.
 * Initially 0.
 */
clock_t last_clock = 0;
/**
 * The music that will be played during the game.
 */
// Mix_Music *gMusic = NULL;

/** Computes the center of the window in pixel coordinates */
vector_t get_window_center(void) {
  int *width = malloc(sizeof(*width)), *height = malloc(sizeof(*height));
  assert(width != NULL);
  assert(height != NULL);
  SDL_GetWindowSize(window, width, height);
  vector_t dimensions = {.x = *width, .y = *height};
  free(width);
  free(height);
  return vec_multiply(0.5, dimensions);
}

/**
 * Computes the scaling factor between scene coordinates and pixel
 coordinates.
* The scene is scaled by the same factor in the x and y dimensions,
* chosen to maximize the size of the scene while keeping it in the window.
*/
double get_scene_scale(vector_t window_center) {
  // Scale scene so it fits entirely in the window
  double x_scale = window_center.x / max_diff.x,
         y_scale = window_center.y / max_diff.y;
  return x_scale < y_scale ? x_scale : y_scale;
}

/** Maps a scene coordinate to a window coordinate */
vector_t get_window_position(vector_t scene_pos, vector_t window_center) {
  // Scale scene coordinates by the scaling factor
  // and map the center of the scene to the center of the window
  vector_t scene_center_offset = vec_subtract(scene_pos, center);
  double scale = get_scene_scale(window_center);
  vector_t pixel_center_offset = vec_multiply(scale, scene_center_offset);
  vector_t pixel = {.x = round(window_center.x + pixel_center_offset.x),
                    // Flip y axis since positive y is down on the screen
                    .y = round(window_center.y - pixel_center_offset.y)};
  return pixel;
}

/** Maps a scene coordinate to a window coordinate */
vector_t get_scene_position(vector_t window_pos, vector_t window_center) {
  // Scale window coordinates by the scaling factor
  double scale = get_scene_scale(window_center);
  window_pos.y = WINDOW_HEIGHT - window_pos.y;
  return vec_multiply(1 / scale, window_pos);
}

/**
 * Converts an SDL key code to a char.
 * 7-bit ASCII characters are just returned
 * and arrow keys are given special character codes.
 */
char get_keycode(SDL_Keycode key) {
  switch (key) {
  case SDLK_LEFT:
    return LEFT_ARROW;
  case SDLK_UP:
    return UP_ARROW;
  case SDLK_RIGHT:
    return RIGHT_ARROW;
  case SDLK_DOWN:
    return DOWN_ARROW;
  case SDLK_c:
    return 'c';
  case SDLK_SPACE:
    return ' ';
  default:
    // Only process 7-bit ASCII characters
    return key == (SDL_Keycode)(char)key ? key : '\0';
  }
}

int get_click_type(uint8_t click) {
  switch (click) {
  case SDL_BUTTON_LEFT:
    return LEFT_CLICK;
  case SDL_BUTTON_MIDDLE:
    return MIDDLE_CLICK;
  case SDL_BUTTON_RIGHT:
    return RIGHT_CLICK;
  default:
    return 0;
  }
}

void sdl_init_with_title(const char *title, vector_t min, vector_t max) {
  // Check parameters
  assert(min.x < max.x);
  assert(min.y < max.y);

  center = vec_multiply(0.5, vec_add(min, max));
  max_diff = vec_subtract(max, center);
  SDL_Init(SDL_INIT_EVERYTHING);
  window =
      SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
}

void sdl_init(vector_t min, vector_t max) {
  sdl_init_with_title(WINDOW_TITLE, min, max);
}

bool sdl_is_done(void *state) {
  SDL_Event *event = malloc(sizeof(*event));
  assert(event != NULL);
  while (SDL_PollEvent(event)) {
    switch (event->type) {
    case SDL_QUIT:
      free(event);
      return true;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      // Skip the keypress if no handler is configured
      // or an unrecognized key was pressed
      if (key_handler == NULL)
        break;
      char key = get_keycode(event->key.keysym.sym);
      if (key == '\0')
        break;

      uint32_t timestamp = event->key.timestamp;
      if (!event->key.repeat) {
        key_start_timestamp = timestamp;
      }
      button_event_type_t type =
          event->type == SDL_KEYDOWN ? BUTTON_PRESSED : BUTTON_RELEASED;
      double held_time = (timestamp - key_start_timestamp) / MS_PER_S;
      key_handler(key, type, held_time, state);
      break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      // Skip the click if no handler is configured
      // or an unrecognized click was pressed
      if (mouse_handler == NULL)
        break;
      uint8_t button = get_click_type(event->button.button);
      if (!button)
        break;

      timestamp = event->button.timestamp;
      if (event->button.state == SDL_RELEASED) {
        click_start_timestamp = 0;
      } else if (!click_start_timestamp) {
        click_start_timestamp = timestamp;
      }
      vector_t position = get_scene_position(
          (vector_t){event->button.x, event->button.y}, get_window_center());
      type =
          event->type == SDL_MOUSEBUTTONDOWN ? BUTTON_PRESSED : BUTTON_RELEASED;
      held_time = (timestamp - click_start_timestamp) / MS_PER_S;
      mouse_handler(button, type, position, held_time, state);
      break;
    }
  }
  free(event);
  return false;
}

void sdl_clear(void) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);
}

void sdl_draw_polygon(body_t *body) {
  rgb_color_t color = body_get_color(body);
  double alpha = body_get_alpha(body);
  list_t *points = body_get_shape(body);
  // Check parameters
  size_t n = list_size(points);
  assert(n >= 3);
  assert(0 <= color.r && color.r <= 1);
  assert(0 <= color.g && color.g <= 1);
  assert(0 <= color.b && color.b <= 1);

  vector_t window_center = get_window_center();

  // Convert each vertex to a point on screen
  int16_t *x_points = malloc(sizeof(*x_points) * n),
          *y_points = malloc(sizeof(*y_points) * n);
  assert(x_points != NULL);
  assert(y_points != NULL);
  for (size_t i = 0; i < n; i++) {
    vector_t *vertex = list_get(points, i);
    vector_t pixel = get_window_position(*vertex, window_center);
    x_points[i] = pixel.x;
    y_points[i] = pixel.y;
  }

  // Draw polygon with the given color
  filledPolygonRGBA(renderer, x_points, y_points, n, color.r * 255,
                    color.g * 255, color.b * 255, alpha * 255);
  list_free(points);
  free(x_points);
  free(y_points);
}

void sdl_show(void) {
  // Draw boundary lines
  vector_t window_center = get_window_center();
  vector_t max = vec_add(center, max_diff),
           min = vec_subtract(center, max_diff);
  vector_t max_pixel = get_window_position(max, window_center),
           min_pixel = get_window_position(min, window_center);
  SDL_Rect *boundary = malloc(sizeof(*boundary));
  boundary->x = min_pixel.x;
  boundary->y = max_pixel.y;
  boundary->w = max_pixel.x - min_pixel.x;
  boundary->h = min_pixel.y - max_pixel.y;
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderDrawRect(renderer, boundary);
  free(boundary);

  SDL_RenderPresent(renderer);
}

void sdl_render_image(SDL_Texture *image, body_t *curr, vector_t dimensions) {
  int w, h;
  vector_t centroid =
      get_window_position(body_get_centroid(curr), get_window_center());
  SDL_QueryTexture(image, NULL, NULL, &w, &h);
  SDL_Rect dims;
  double scale = get_scene_scale(get_window_center());
  if (dimensions.x != 0 && dimensions.y != 0) {
    dims.w = dimensions.x * scale;
    dims.h = dimensions.y * scale;
  } else {
    dims.w = w * DEFAULT_IMG_SCALE; // TODO: Fix Scale Factor
    dims.h = h * DEFAULT_IMG_SCALE;
  }
  dims.x = centroid.x - dims.w / 2;
  dims.y = centroid.y - dims.h / 2;
  SDL_RenderCopyEx(renderer, image, NULL, &dims,
                   -body_get_angle(curr) * 180 / M_PI, NULL, SDL_FLIP_NONE);
}

SDL_Texture *sdl_load_image(char *image_path) {
  SDL_Texture *image = IMG_LoadTexture(renderer, image_path);
  return image;
}

void sdl_render_scene(scene_t *scene) { // check if body has a sprite and then
                                        // either display sprite or shape
  sdl_clear();
  size_t body_count = scene_bodies(scene);
  for (size_t i = 0; i < body_count; i++) {
    body_t *curr = scene_get_body(scene, i);
    if (!body_hidden(curr)) {
      SDL_Texture *image = body_get_image(curr);
      SDL_Texture *shadow = body_get_shadow(curr);
      if (shadow != NULL) {
        sdl_render_image(
            shadow, curr,
            vec_multiply(DEFAULT_SHADOW_SCALE, body_get_dimensions(curr)));
      }
      if (image != NULL) {
        sdl_render_image(image, curr, body_get_dimensions(curr));
      } else {
        sdl_draw_polygon(curr);
      }
    }
  }
  sdl_show();
}

void sdl_on_key(key_handler_t handler) { key_handler = handler; }

void sdl_on_click(mouse_handler_t handler) { mouse_handler = handler; }

double time_since_last_tick(void) {
  clock_t now = clock();
  double difference = last_clock
                          ? (double)(now - last_clock) / CLOCKS_PER_SEC
                          : 0.0; // return 0 the first time this is called
  last_clock = now;
  return difference;
}

vector_t sdl_mouse_handler(body_t *cue_ball) {
  vector_t mouse_position = VEC_ZERO;
  int x, y;
  Uint32 buttons = 0;
  SDL_PumpEvents();
  buttons = SDL_GetMouseState(&x, &y);
  // vector_t cue_ball_pos = body_get_centroid(cue_ball);
  vector_t cue_ball_pos = body_get_centroid(cue_ball);
  // Also include game window dimensions
  if ((buttons & SDL_BUTTON_LMASK)) {
    mouse_position.x = x;
    mouse_position.y = y;
  } else {
    return VEC_ZERO;
  }
  mouse_position = get_scene_position(mouse_position, get_window_center());
  vector_t vector_from_centroid =
      (vec_unit(vec_negate(vec_subtract(mouse_position, cue_ball_pos))));
  return vector_from_centroid;
}