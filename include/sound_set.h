#ifndef __SOUND_SET_H__
#define __SOUND_SET_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * Describes a set of SDL_Mixer objects used to create sound effects in the game
 *
 */
typedef struct sound_set sound_set_t;

/**
 * Initialize a set of sound effects for different collisions
 *
 * @param ball_ball collision sound for ball-ball collision
 * @param cue_ball collision sound for cue-ball collision
 * @param pocket_ball collision sound for pocket-ball collision
 * @param wall_ball collision osund for wall-ball collision
 */
sound_set_t *sound_set_init(Mix_Chunk *ball_ball, Mix_Chunk *cue_ball,
                            Mix_Chunk *pocket_ball, Mix_Chunk *wall_ball);

Mix_Chunk *get_ball_ball(sound_set_t *sound_set);
Mix_Chunk *get_cue_ball(sound_set_t *sound_set);
Mix_Chunk *get_pocket_ball(sound_set_t *sound_set);
Mix_Chunk *get_wall_ball(sound_set_t *sound_set);

void sound_set_toggle_muted(sound_set_t *sound_set);

bool sound_set_get_muted(sound_set_t *sound_set);

void sound_set_free(sound_set_t *sound_set);

#endif