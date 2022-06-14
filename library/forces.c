#include "forces.h"
#include "collision.h"
#include "sound_set.h"
#include <SDL2/SDL_mixer.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

const double FRICTION_THRESHOLD = 1e0;

typedef struct {
  double constant;
  list_t *bodies;
} aux_t;

typedef struct {
  void *aux;
  free_func_t freer;
  list_t *bodies;
  collision_handler_t handler;
  collision_sound_handler_t sound_handler;
  bool collided;
  sound_set_t *sound_set;
} collision_aux_t;

void aux_freer(aux_t *aux) {
  list_free(aux->bodies);
  free(aux);
}

void collision_aux_freer(collision_aux_t *aux) {
  if (aux->freer != NULL) {
    aux->freer(aux->aux);
  }
  list_free(aux->bodies);
  free(aux);
}

void newtonian_gravity_helper(void *aux) {
  int min_distance = 5;

  assert(list_size(((aux_t *)aux)->bodies) == 2);
  body_t *body1 = list_get(((aux_t *)aux)->bodies, 0);
  body_t *body2 = list_get(((aux_t *)aux)->bodies, 1);
  double G = ((aux_t *)aux)->constant;

  vector_t difference =
      vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
  double distance = vec_magnitude(difference) > min_distance
                        ? vec_magnitude(difference)
                        : min_distance;
  double magnitude =
      G * body_get_mass(body1) * body_get_mass(body2) / (distance * distance);
  vector_t force = vec_init(magnitude, vec_direction(difference));
  body_add_force(body1, force);
  body_add_force(body2, vec_negate(force));
}

void create_newtonian_gravity(scene_t *scene, double G, body_t *body1,
                              body_t *body2) {
  aux_t *aux = malloc(sizeof(aux_t));
  aux->constant = G;
  aux->bodies = list_init(2, NULL);
  list_add(aux->bodies, body1);
  list_add(aux->bodies, body2);
  list_t *bodies = list_init(2, NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  scene_add_bodies_force_creator(scene,
                                 (force_creator_t)newtonian_gravity_helper, aux,
                                 bodies, (free_func_t)aux_freer);
}

void spring_helper(void *aux) {
  assert(list_size(((aux_t *)aux)->bodies) == 2);
  body_t *body1 = list_get(((aux_t *)aux)->bodies, 0);
  body_t *body2 = list_get(((aux_t *)aux)->bodies, 1);
  double k = ((aux_t *)aux)->constant;

  vector_t difference =
      vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
  double magnitude = k * vec_magnitude(difference);
  vector_t force = vec_init(magnitude, vec_direction(difference));
  body_add_force(body1, force);
  body_add_force(body2, vec_negate(force));
}

void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2) {
  aux_t *aux = malloc(sizeof(aux_t));
  aux->constant = k;
  aux->bodies = list_init(2, NULL);
  list_add(aux->bodies, body1);
  list_add(aux->bodies, body2);
  list_t *bodies = list_init(2, NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  scene_add_bodies_force_creator(scene, (force_creator_t)spring_helper, aux,
                                 bodies, (free_func_t)aux_freer);
}

void drag_helper(void *aux) {
  assert(list_size(((aux_t *)aux)->bodies) == 1);
  body_t *body = list_get(((aux_t *)aux)->bodies, 0);
  double gamma = ((aux_t *)aux)->constant;

  vector_t v = body_get_velocity(body);
  double magnitude = gamma * vec_magnitude(v) * vec_magnitude(v);
  vector_t force = vec_init(-magnitude, vec_direction(v));
  body_add_force(body, force);
}

void create_drag(scene_t *scene, double gamma, body_t *body) {
  aux_t *aux = malloc(sizeof(aux_t));
  aux->constant = gamma;
  aux->bodies = list_init(1, NULL);
  list_add(aux->bodies, body);
  list_t *bodies = list_init(1, NULL);
  list_add(bodies, body);
  scene_add_bodies_force_creator(scene, (force_creator_t)drag_helper, aux,
                                 bodies, (free_func_t)aux_freer);
}

void gravity_friction_helper(void *aux) {
  assert(list_size(((aux_t *)aux)->bodies) == 1);
  body_t *body = list_get(((aux_t *)aux)->bodies, 0);
  double mu_x_g = ((aux_t *)aux)->constant;

  vector_t v = body_get_velocity(body);
  if (vec_magnitude(v) < FRICTION_THRESHOLD) {
    body_set_velocity(body, VEC_ZERO);
  } else {
    double magnitude = mu_x_g * body_get_mass(body);
    vector_t force = vec_init(-magnitude, vec_direction(v));
    body_add_force(body, force);
  }
}

void create_gravity_friction(scene_t *scene, double mu_x_g, body_t *body) {
  aux_t *aux = malloc(sizeof(aux_t));
  aux->constant = mu_x_g;
  aux->bodies = list_init(1, NULL);
  list_add(aux->bodies, body);
  list_t *bodies = list_init(1, NULL);
  list_add(bodies, body);
  scene_add_bodies_force_creator(scene,
                                 (force_creator_t)gravity_friction_helper, aux,
                                 bodies, (free_func_t)aux_freer);
}

void destructive_collision_handler(body_t *body1, body_t *body2, vector_t axis,
                                   void *aux) {
  body_remove(body1);
  body_remove(body2);
}

void create_destructive_collision(scene_t *scene, body_t *body1,
                                  body_t *body2) {
  aux_t *aux = malloc(sizeof(aux_t));
  aux->bodies = list_init(0, NULL);
  create_collision(scene, body1, body2, destructive_collision_handler, aux,
                   (free_func_t)aux_freer);
}

void breaking_collision_handler(body_t *body1, body_t *body2, vector_t axis,
                                void *aux) {
  double elasticity = ((aux_t *)aux)->constant;
  double J = -body_get_mass(body1) * (1 + elasticity) *
             vec_dot(body_get_velocity(body1), axis);
  vector_t impulse = vec_multiply(J, axis);
  body_add_impulse(body1, impulse);
  body_remove(body2);
}

void create_breaking_collision(scene_t *scene, double elasticity, body_t *body1,
                               body_t *body2) {
  aux_t *aux = malloc(sizeof(aux_t));
  aux->bodies = list_init(0, NULL);
  aux->constant = elasticity;
  create_collision(scene, body1, body2, breaking_collision_handler, aux,
                   (free_func_t)aux_freer);
}

void physics_collision_handler(body_t *body1, body_t *body2, vector_t axis,
                               void *aux) {
  vector_t v1 = body_get_velocity(body1);
  vector_t v2 = body_get_velocity(body2);
  double elasticity = ((aux_t *)aux)->constant;
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
}

void create_physics_collision(scene_t *scene, double elasticity, body_t *body1,
                              body_t *body2) {
  aux_t *aux = malloc(sizeof(aux_t));
  aux->bodies = list_init(0, NULL);
  aux->constant = elasticity;
  create_collision(scene, body1, body2, physics_collision_handler, aux,
                   (free_func_t)aux_freer);
}

void create_physics_collision_with_sound(
    scene_t *scene, double elasticity, body_t *body1, body_t *body2,
    collision_sound_handler_t sound_handler) {
  aux_t *aux = malloc(sizeof(aux_t));
  aux->bodies = list_init(0, NULL);
  aux->constant = elasticity;
  create_collision_with_sound(scene, body1, body2, physics_collision_handler,
                              sound_handler, aux, (free_func_t)aux_freer);
}

// body 1 = pocket
// body 2 = ball
// void pocket_collision_handler(body_t *body1, body_t *body2, vector_t axis,
//                               void *aux) {
//   if (body_get_respawnable(body2)) {
//     body_set_to_respawn(body2, true);
//   } else {
//     body_remove(body2);
//   }
// }

// void create_pocket_collision_with_sound(
//     scene_t *scene, body_t *pocket, body_t *ball,
//     collision_sound_handler_t sound_handler) {
//   aux_t *aux = malloc(sizeof(aux_t));
//   aux->bodies = list_init(0, NULL);
//   int *info = body_get_info(ball);
//   aux->constant = *info;
//   create_collision_with_sound(scene, pocket, ball, pocket_collision_handler,
//                               sound_handler, aux, (free_func_t)aux_freer);
// }

void collision_helper(void *aux) {
  collision_aux_t *c_aux = aux;
  assert(list_size(c_aux->bodies) == 2);
  body_t *body1 = list_get(c_aux->bodies, 0);
  body_t *body2 = list_get(c_aux->bodies, 1);

  list_t *shape1 = body_get_shape(body1);
  list_t *shape2 = body_get_shape(body2);
  collision_info_t collision = find_collision(shape1, shape2);
  if (collision.collided) {
    if (!c_aux->collided) {
      c_aux->handler(body1, body2, collision.axis, c_aux->aux);
      if (c_aux->sound_handler != NULL) {
        c_aux->sound_handler(c_aux->sound_set, body1, body2);
      }
      c_aux->collided = true;
    }
  } else {
    c_aux->collided = false;
  }
  list_free(shape1);
  list_free(shape2);
}

void create_collision(scene_t *scene, body_t *body1, body_t *body2,
                      collision_handler_t handler, void *aux,
                      free_func_t freer) {
  create_collision_with_sound(scene, body1, body2, handler, NULL, aux, freer);
}

void create_collision_with_sound(scene_t *scene, body_t *body1, body_t *body2,
                                 collision_handler_t handler,
                                 collision_sound_handler_t sound_handler,
                                 void *aux, free_func_t freer) {
  collision_aux_t *c_aux = malloc(sizeof(collision_aux_t));
  c_aux->aux = aux;
  c_aux->freer = freer;
  c_aux->bodies = list_init(2, NULL);
  list_add(c_aux->bodies, body1);
  list_add(c_aux->bodies, body2);
  c_aux->handler = handler;
  c_aux->sound_handler = sound_handler;
  c_aux->collided = false;
  c_aux->sound_set = scene_get_sound_set(scene);
  list_t *bodies = list_init(2, NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  scene_add_bodies_force_creator(scene, collision_helper, c_aux, bodies,
                                 (free_func_t)collision_aux_freer);
}
