#include "scene.h"
#include "sound_set.h"
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>

const int INITIAL_SIZE = 20;
const int INITIAL_FORCE_NUM = 10;

const int STD_FREQUENCY = 44100;
const int STD_CHANNELS = 2;
const int STD_CHUNKSIZE = 2048;

typedef struct {
  force_creator_t forcer;
  void *aux;
  list_t *bodies;
  free_func_t freer;
} force_t;

typedef struct scene {
  list_t *bodies;
  list_t *forces;
  double time;
  Mix_Music *music;
  sound_set_t *sound_set;
} scene_t;

void force_free(force_t *force) {
  if (force->freer != NULL) {
    force->freer(force->aux);
  }
  list_free(force->bodies);
  free(force);
}

scene_t *scene_init_with_audio(const char *music_path) {
  scene_t *scene = malloc(sizeof(scene_t));
  scene->bodies = list_init(INITIAL_SIZE, (free_func_t)body_free);
  scene->forces = list_init(INITIAL_FORCE_NUM, (free_func_t)force_free);
  scene->time = 0;
  scene->sound_set = NULL;
  Mix_OpenAudio(STD_FREQUENCY, MIX_DEFAULT_FORMAT, STD_CHANNELS, STD_CHUNKSIZE);
  scene->music = Mix_LoadMUS(music_path);
  Mix_PlayMusic(scene->music, -1);
  return scene;
}

scene_t *scene_init(void) { return scene_init_with_audio(NULL); }

void scene_add_sound_set(scene_t *scene, const char *bb_path,
                         const char *cb_path, const char *pb_path,
                         const char *wb_path) {
  scene->sound_set = sound_set_init(Mix_LoadWAV(bb_path), Mix_LoadWAV(cb_path),
                                    Mix_LoadWAV(pb_path), Mix_LoadWAV(wb_path));
}

void scene_free(scene_t *scene) {
  list_free(scene->bodies);
  list_free(scene->forces);
  if (scene->sound_set != NULL) {
    sound_set_free(scene->sound_set);
  }
  Mix_FreeMusic(scene->music);
  Mix_Quit();
  free(scene);
}

size_t scene_bodies(scene_t *scene) { return list_size(scene->bodies); }

body_t *scene_get_body(scene_t *scene, size_t index) {
  return list_get(scene->bodies, index);
}

void scene_add_body(scene_t *scene, body_t *body) {
  list_add(scene->bodies, body);
}

void scene_remove_body(scene_t *scene, size_t index) {
  body_remove(list_get(scene->bodies, index));
  // body_free(list_get(scene->bodies, index));
  // list_remove(scene->bodies, index);
}

void scene_add_force_creator(scene_t *scene, force_creator_t forcer, void *aux,
                             free_func_t freer) {
  scene_add_bodies_force_creator(scene, forcer, aux, list_init(0, NULL), freer);
}

void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer,
                                    void *aux, list_t *bodies,
                                    free_func_t freer) {
  force_t *force = malloc(sizeof(force_t));
  force->forcer = forcer;
  force->aux = aux;
  force->bodies = bodies;
  force->freer = freer;
  list_add(scene->forces, force);
}

void scene_tick(scene_t *scene, double dt) {
  scene->time += dt;
  for (size_t i = 0; i < list_size(scene->forces); i++) {
    force_t *curr = list_get(scene->forces, i);
    bool apply_force = true;
    for (size_t j = 0; j < list_size(curr->bodies); j++) {
      if (!body_get_apply_forces(list_get(curr->bodies, j))) {
        apply_force = false;
        break;
      }
    }
    if (apply_force)
      curr->forcer(curr->aux);
  }
  for (size_t i = 0; i < list_size(scene->forces); i++) {
    force_t *curr = list_get(scene->forces, i);
    for (size_t j = 0; j < list_size(curr->bodies); j++) {
      if (body_is_removed(list_get(curr->bodies, j))) {
        force_free(list_get(scene->forces, i));
        list_remove(scene->forces, i);
        i--;
        break;
      }
    }
  }
  for (size_t i = 0; i < list_size(scene->bodies); i++) {
    body_t *curr = list_get(scene->bodies, i);
    if (body_is_removed(curr)) {
      body_free(curr);
      list_remove(scene->bodies, i);
      i--;
    } else {
      body_tick(curr, dt);
    }
  }
}

double scene_get_time(scene_t *scene) { return scene->time; }

void scene_reset_time(scene_t *scene) { scene->time = 0; }

int scene_get_index(scene_t *scene, body_t *body) {
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    if (scene_get_body(scene, i) == body) {
      return i;
    }
  }
  return -1;
}

bool scene_is_still(scene_t *scene) {
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_t *curr = scene_get_body(scene, i);
    if (!vec_is_within(1e0, body_get_velocity(curr), VEC_ZERO)) {
      return false;
    }
  }
  return true;
}

void scene_toggle_muted(scene_t *scene) {
  sound_set_toggle_muted(scene->sound_set);
  if (Mix_PausedMusic()) {
    Mix_ResumeMusic();
  } else {
    Mix_PauseMusic();
  }
}

sound_set_t *scene_get_sound_set(scene_t *scene) { return scene->sound_set; }

Mix_Chunk *scene_get_ball_ball(scene_t *scene) {
  return get_ball_ball(scene->sound_set);
}

Mix_Chunk *scene_get_cue_ball(scene_t *scene) {
  return get_cue_ball(scene->sound_set);
}

Mix_Chunk *scene_get_pocket_ball(scene_t *scene) {
  return get_pocket_ball(scene->sound_set);
}

Mix_Chunk *scene_get_wall_ball(scene_t *scene) {
  return get_wall_ball(scene->sound_set);
}