#include "color.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

rgb_color_t *create_rainbow(int steps) {
  // if (steps % 6 != 0) {
  //   rgb_color_t *temp_rainbow = create_rainbow(steps*6);
  //   rgb_color_t *rainbow = malloc(steps * sizeof(rgb_color_t));
  //   for (int i = 0; i < steps; i++) {
  //     rainbow[i] = temp_rainbow[6*i];
  //   }
  //   return rainbow;
  // }

  double rgb[] = {1.0, 0, 0};
  rgb_color_t *rainbow = malloc(6 * steps * sizeof(rgb_color_t));
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < steps * 2; j++) {
      rainbow[2 * steps * i + j] =
          (rgb_color_t){fabs(rgb[0]), fabs(rgb[1]), fabs(rgb[2])};
      if (j < steps) {
        rgb[(i + 1) % 3] += 1.0 / steps;
      } else {
        rgb[i] -= 1.0 / steps;
      }
    }
  }
  return rainbow;
}
