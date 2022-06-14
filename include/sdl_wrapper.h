#ifndef __SDL_WRAPPER_H__
#define __SDL_WRAPPER_H__

#include "body.h"
#include "color.h"
#include "list.h"
#include "scene.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

// Values passed to a key handler when the given arrow key is pressed
typedef enum {
  LEFT_ARROW = 1,
  UP_ARROW = 2,
  RIGHT_ARROW = 3,
  DOWN_ARROW = 4
} arrow_key_t;

typedef enum {
  LEFT_CLICK = 1,
  MIDDLE_CLICK = 2,
  RIGHT_CLICK = 3,
} mouse_click_t;

/**
 * The possible types of key events.
 * Enum types in C are much more primitive than in Java; this is equivalent
 to:
 * typedef unsigned int KeyEventType;
 * #define BUTTON_PRESSED 0
 * #define BUTTON_RELEASED 1
 */
typedef enum { BUTTON_PRESSED, BUTTON_RELEASED } button_event_type_t;

/**
 * A keypress handler.
 * When a key is pressed or released, the handler is passed its char value.
 * Most keys are passed as their char value, e.g. 'a', '1', or '\r'.
 * Arrow keys have the special values listed above.
 *
 * @param key a character indicating which key was pressed
 * @param type the type of key event (BUTTON_PRESSED or BUTTON_RELEASED)
 * @param held_time if a press event, the time the key has been held in
 seconds
 */
typedef void (*key_handler_t)(char key, button_event_type_t type,
                              double held_time, void *state);

/**
 * A mouse click handler.
 * When a mouse button is pressed or released, the handler is passed its char
 value.
 * Most keys are passed as their char value, e.g. 'a', '1', or '\r'.
 * Arrow keys have the special values listed above.
 *
 * @param type the type of key event (BUTTON_PRESSED or BUTTON_RELEASED)
 * @param position the location of the click
 * @param held_time if a press event, the time the key has been held in
 seconds
 */
typedef void (*mouse_handler_t)(int button, button_event_type_t type,
                                vector_t position, double held_time,
                                void *state);

/**
 * Initializes the SDL window and renderer.
 * Must be called once before any of the other SDL functions.
 *
 * @param min the x and y coordinates of the bottom left of the scene
 * @param max the x and y coordinates of the top right of the scene
 */
void sdl_init(vector_t min, vector_t max);

/**
 * Initializes the SDL window and renderer with a given title.
 * Must be called once before any of the other SDL functions.
 *
 * @param title the window's title
 * @param min the x and y coordinates of the bottom left of the scene
 * @param max the x and y coordinates of the top right of the scene
 */
void sdl_init_with_title(const char *title, vector_t min, vector_t max);

/**
 * Processes all SDL events and returns whether the window has been closed.
 * This function must be called in order to handle keypresses.
 *
 * @return true if the window was closed, false otherwise
 */
bool sdl_is_done(void *state);

/**
 * Clears the screen. Should be called before drawing polygons in each frame.
 */
void sdl_clear(void);

/**
 * Draws a polygon from the given list of vertices and a color.
 *
 * @param points the list of vertices of the polygon
 * @param color the color used to fill in the polygon
 */
void sdl_draw_polygon(body_t *body);

/**
 * Displays the rendered frame on the SDL window.
 * Must be called after drawing the polygons in order to show them.
 */
void sdl_show(void);

/**
 * Loads an image as an SDL_Texture.
 * Returns an SDL_Texture.
 */
SDL_Texture *sdl_load_image(char *image_path);

// /**
//  * Rotates an SDL_texture.
//  */
// void sdl_rotate_image(body_t *curr, double angle);

/**
 * Draws all bodies in a scene.
 * This internally calls sdl_clear(), sdl_draw_polygon(), and sdl_show(),
 * so those functions should not be called directly.
 *
 * @param scene the scene to draw
 */
void sdl_render_scene(scene_t *scene);

/**
 * Registers a function to be called every time a key is pressed.
 * Overwrites any existing handler.
 *
 * Example:
 * ```
 * void on_key(char key, button_event_type_t type, double held_time) {
 *     if (type == BUTTON_PRESSED) {
 *         switch (key) {
 *             case 'a':
 *                 printf("A pressed\n");
 *                 break;
 *             case UP_ARROW:
 *                 printf("UP pressed\n");
 *                 break;
 *         }
 *     }
 * }
 * int main(void) {
 *     sdl_on_key(on_key);
 *     while (!sdl_is_done());
 * }
 * ```
 *
 * @param handler the function to call with each key press
 */
void sdl_on_key(key_handler_t handler);

/**
 * Registers a function to be called every time a mouse button is clicked.
 * Overwrites any existing handler.
 *
 * See format for sdl_on_key above.
 *
 * @param handler the function to call with each mouse click
 */
void sdl_on_click(mouse_handler_t handler);

/**
 * Gets the amount of time that has passed since the last time
 * this function was called, in seconds.
 *
 * @return the number of seconds that have elapsed
 */
double time_since_last_tick(void);

/**
 * Returns a unit vector from the centroid based on a mouse click.
 *
 * @param cue_ball the cue ball where the vector will be centered out
 * @returns a vector based on the mouse click for where the cue ball should
 * travel
 */
vector_t sdl_mouse_handler(body_t *cue_ball);

#endif // #ifndef __SDL_WRAPPER_H__
