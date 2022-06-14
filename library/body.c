#include "color.h"
#include "list.h"
#include "polygon.h"
#include "sdl_wrapper.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct body {
  list_t *shape;
  vector_t centroid, velocity, force, impulse;
  rgb_color_t color;
  double mass, angle, alpha;
  void *info;
  free_func_t info_freer;
  bool is_removed, to_respawn, respawnable, hidden, apply_forces;
  SDL_Texture *image;
  SDL_Texture *shadow;
  vector_t dimensions;
} body_t;

body_t *body_init_with_info_and_sprite(list_t *shape, double mass,
                                       rgb_color_t color, void *info,
                                       SDL_Texture *image,
                                       free_func_t info_freer) {
  assert(mass != 0);
  body_t *body = malloc(sizeof(body_t));
  body->shape = shape;
  body->centroid = polygon_centroid(shape);
  body->velocity = VEC_ZERO;
  body->force = VEC_ZERO;
  body->impulse = VEC_ZERO;
  body->color = color;
  body->mass = mass;
  body->angle = 0;
  body->alpha = 1;
  body->info = info;
  body->info_freer = info_freer;
  body->is_removed = false;
  body->to_respawn = false;
  body->respawnable = false;
  body->hidden = false;
  body->apply_forces = true;
  body->image = image;
  body->shadow = NULL;
  body->dimensions = VEC_ZERO;

  return body;
}

body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color,
                            void *info, free_func_t info_freer) {
  return body_init_with_info_and_sprite(shape, mass, color, info, NULL,
                                        info_freer);
}

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
  return body_init_with_info(shape, mass, color, NULL, free);
}

void body_free(body_t *body) {
  list_free(body->shape);
  if (body->info_freer != NULL) {
    body->info_freer(body->info);
  }
  if (body->image != NULL) {
    SDL_DestroyTexture(body->image);
  }
  free(body);
}

list_t *body_get_shape(body_t *body) { return polygon_copy(body->shape); }

double body_get_angle(body_t *body) { return body->angle; }

vector_t body_get_centroid(body_t *body) { return body->centroid; }

vector_t body_get_center(body_t *body) { return polygon_center(body->shape); }

vector_t body_get_velocity(body_t *body) { return body->velocity; }

double body_get_mass(body_t *body) { return body->mass; }

rgb_color_t body_get_color(body_t *body) { return body->color; }

void *body_get_info(body_t *body) { return body->info; }

SDL_Texture *body_get_image(body_t *body) { return body->image; }

SDL_Texture *body_get_shadow(body_t *body) { return body->shadow; }

vector_t body_get_dimensions(body_t *body) { return body->dimensions; }

void body_set_velocity(body_t *body, vector_t v) { body->velocity = v; }

void body_set_color(body_t *body, rgb_color_t color) { body->color = color; }

void body_set_dimensions(body_t *body, vector_t dimensions) {
  body->dimensions = dimensions;
}

void body_set_image(body_t *body, SDL_Texture *image) { body->image = image; }

void body_set_shadow(body_t *body, SDL_Texture *shadow) {
  body->shadow = shadow;
}

void body_rotate_about_point(body_t *body, double angle, vector_t point) {
  polygon_rotate(body->shape, angle, point);
  body->angle += angle;
  body->centroid = polygon_centroid(body->shape);
}

void body_rotate(body_t *body, double angle) {
  polygon_rotate(body->shape, angle, body->centroid);
  body->angle += angle;
}

void body_set_rotation(body_t *body, double angle) {
  body_rotate(body, angle - body->angle);
}

void body_translate(body_t *body, vector_t displacement) {
  polygon_translate(body->shape, displacement);
  body->centroid = vec_add(body->centroid, displacement);
}

void body_set_centroid(body_t *body, vector_t v) {
  body_translate(body, vec_subtract(v, body->centroid));
}

void body_add_force(body_t *body, vector_t force) {
  body->force = vec_add(body->force, force);
}

void body_add_impulse(body_t *body, vector_t impulse) {
  body->impulse = vec_add(body->impulse, impulse);
}

void body_tick(body_t *body, double dt) {
  vector_t v_old = body->velocity;
  body->velocity = vec_add(body->velocity,
                           vec_multiply(dt / body_get_mass(body), body->force));
  body->velocity = vec_add(
      body->velocity, vec_multiply(1 / body_get_mass(body), body->impulse));
  body->force = VEC_ZERO;
  body->impulse = VEC_ZERO;

  vector_t v_avg = vec_multiply(0.5, vec_add(v_old, body->velocity));
  body_translate(body, vec_multiply(dt, v_avg));
}

void body_remove(body_t *body) { body->is_removed = true; }

bool body_is_removed(body_t *body) { return body->is_removed; }

void body_stretch_x(body_t *body, double factor) {
  vector_t old_centroid = body_get_centroid(body);
  polygon_stretch_x(body->shape, factor);
  body->centroid = polygon_centroid(body->shape);
  body_set_centroid(body, old_centroid);
}

void body_set_respawnable(body_t *body, bool respawanable) {
  body->respawnable = respawanable;
}

bool body_get_respawnable(body_t *body) { return body->respawnable; }

void body_set_to_respawn(body_t *body, bool to_respawn) {
  body->to_respawn = to_respawn;
}

bool body_to_respawn(body_t *body) { return body->to_respawn; }

void body_hide(body_t *body, bool hidden) { body->hidden = hidden; }

bool body_hidden(body_t *body) { return body->hidden; }

void body_set_apply_forces(body_t *body, bool apply_forces) {
  body->apply_forces = apply_forces;
}

bool body_get_apply_forces(body_t *body) { return body->apply_forces; }

void body_set_alpha(body_t *body, double alpha) {
  assert(0 <= alpha && alpha <= 1);
  body->alpha = alpha;
}

double body_get_alpha(body_t *body) { return body->alpha; }