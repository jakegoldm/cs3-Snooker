#ifndef __COLOR_H__
#define __COLOR_H__

/**
 * A color to display on the screen.
 * The color is represented by its red, green, and blue components.
 * Each component must be between 0 (black) and 1 (white).
 */
typedef struct {
  float r;
  float g;
  float b;
} rgb_color_t;

/**
 * Make an array with the colors of the rainbow.
 *
 * @param steps the number of colors in between each step of r->y, y->g, etc.
 * @return a pointer to the array of colors in the rainbow. Will have a length
 * of steps
 */
rgb_color_t *create_rainbow(int steps);

static const rgb_color_t BLACK = {0, 0, 0};
static const rgb_color_t WHITE = {1, 1, 1};
static const rgb_color_t LIGHT_GRAY = {0.75, 0.75, 0.75};
static const rgb_color_t GRAY = {0.5, 0.5, 0.5};
static const rgb_color_t DARK_GRAY = {0.25, 0.25, 0.25};
static const rgb_color_t BROWN = {0.5, 0.25, 0.1};

static const rgb_color_t RED = {1, 0, 0};
static const rgb_color_t ORANGE = {1, 0.5, 0};
static const rgb_color_t YELLOW = {1, 1, 0};
static const rgb_color_t GREEN = {0, 1, 0};
static const rgb_color_t CYAN = {0, 1, 1};
static const rgb_color_t BLUE = {0, 0, 1};
static const rgb_color_t MAGENTA = {1, 0, 1};
static const rgb_color_t PINK = {1, 0.5, 0.5};

#endif // #ifndef __COLOR_H__
