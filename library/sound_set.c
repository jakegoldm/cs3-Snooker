#include "sound_set.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct sound_set {
  Mix_Chunk *ball_ball, *cue_ball, *pocket_ball, *wall_ball;
  bool muted;
} sound_set_t;

sound_set_t *sound_set_init(Mix_Chunk *ball_ball, Mix_Chunk *cue_ball,
                            Mix_Chunk *pocket_ball, Mix_Chunk *wall_ball) {
  sound_set_t *sound_set = malloc(sizeof(sound_set_t));
  sound_set->ball_ball = ball_ball;
  sound_set->cue_ball = cue_ball;
  sound_set->pocket_ball = pocket_ball;
  sound_set->wall_ball = wall_ball;
  sound_set->muted = false;
  return sound_set;
}

Mix_Chunk *get_ball_ball(sound_set_t *sound_set) {
  return sound_set->ball_ball;
}

Mix_Chunk *get_cue_ball(sound_set_t *sound_set) { return sound_set->cue_ball; }

Mix_Chunk *get_pocket_ball(sound_set_t *sound_set) {
  return sound_set->pocket_ball;
}

Mix_Chunk *get_wall_ball(sound_set_t *sound_set) {
  return sound_set->wall_ball;
}

void sound_set_toggle_muted(sound_set_t *sound_set) {
  sound_set->muted = !(sound_set->muted);
}

bool sound_set_get_muted(sound_set_t *sound_set) { return sound_set->muted; }

void sound_set_free(sound_set_t *sound_set) {
  if (sound_set->ball_ball != NULL) {
    Mix_FreeChunk(sound_set->ball_ball);
  }
  if (sound_set->cue_ball != NULL) {
    Mix_FreeChunk(sound_set->cue_ball);
  }
  if (sound_set->pocket_ball != NULL) {
    Mix_FreeChunk(sound_set->pocket_ball);
  }
  if (sound_set->wall_ball != NULL) {
    Mix_FreeChunk(sound_set->wall_ball);
  }
  free(sound_set);
}