#ifndef __SCENE_H__
#define __SCENE_H__

#include "body.h"
#include "list.h"
#include "sound_set.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

/**
 * A collection of bodies and force creators.
 * The scene automatically resizes to store
 * arbitrarily many bodies and force creators.
 */
typedef struct scene scene_t;

/**
 * A function which adds some forces or impulses to bodies,
 * e.g. from collisions, gravity, or spring forces.
 * Takes in an auxiliary value that can store parameters or state.
 */
typedef void (*force_creator_t)(void *aux);

/**
 * Allocates memory for an empty scene with background audio.
 * Makes a reasonable guess of the number of bodies to allocate space for.
 * Asserts that the required memory is successfully allocated.
 *
 * @param music_path the string path to the background audio file
 * @return the new scene
 */
scene_t *scene_init_with_audio(const char *music_path);

/**
 * Allocates memory for an empty scene.
 * Makes a reasonable guess of the number of bodies to allocate space for.
 * Asserts that the required memory is successfully allocated.
 *
 * @return the new scene
 */
scene_t *scene_init(void);

/**
 * Initializes a sound_set of set of sound effects for a scene.
 *
 * @param scene scene pointer returned from scene_init_with_audio();
 * @param bb_path string path to the ball-ball collision sound effect
 * @param cb_path string path to the cue-ball collision sound effect
 * @param pb_path string path to the pocket-ball collision sound effect
 * @param wb_path string path to the wall-ball collision sound effect
 */
void scene_add_sound_set(scene_t *scene, const char *bb_path,
                         const char *cb_path, const char *pb_path,
                         const char *wb_path);

/**
 * Releases memory allocated for a given scene
 * and all the bodies and force creators it contains.
 *
 * @param scene a pointer to a scene returned from scene_init()
 */
void scene_free(scene_t *scene);

/**
 * Gets the number of bodies in a given scene.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @return the number of bodies added with scene_add_body()
 */
size_t scene_bodies(scene_t *scene);

/**
 * Gets the body at a given index in a scene.
 * Asserts that the index is valid.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param index the index of the body in the scene (starting at 0)
 * @return a pointer to the body at the given index
 */
body_t *scene_get_body(scene_t *scene, size_t index);

/**
 * Adds a body to a scene.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param body a pointer to the body to add to the scene
 */
void scene_add_body(scene_t *scene, body_t *body);

/**
 * @deprecated Use body_remove() instead
 *
 * Removes and frees the body at a given index from a scene.
 * Asserts that the index is valid.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param index the index of the body in the scene (starting at 0)
 */
void scene_remove_body(scene_t *scene, size_t index);

/**
 * @deprecated Use scene_add_bodies_force_creator() instead
 * so the scene knows which bodies the force creator depends on
 */
void scene_add_force_creator(scene_t *scene, force_creator_t forcer, void *aux,
                             free_func_t freer);

/**
 * Adds a force creator to a scene,
 * to be invoked every time scene_tick() is called.
 * The auxiliary value is passed to the force creator each time it is called.
 * The force creator is registered with a list of bodies it applies to,
 * so it can be removed when any one of the bodies is removed.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param forcer a force creator function
 * @param aux an auxiliary value to pass to forcer when it is called
 * @param bodies the list of bodies affected by the force creator.
 *   The force creator will be removed if any of these bodies are removed.
 *   This list does not own the bodies, so its freer should be NULL.
 * @param freer if non-NULL, a function to call in order to free aux
 */
void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer,
                                    void *aux, list_t *bodies,
                                    free_func_t freer);

/**
 * Executes a tick of a given scene over a small time interval.
 * This requires executing all the force creators
 * and then ticking each body (see body_tick()).
 * If any bodies are marked for removal, they should be removed from the scene
 * and freed, along with any force creators acting on them.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param dt the time elapsed since the last tick, in seconds
 */
void scene_tick(scene_t *scene, double dt);

/**
 * @brief Get the time object
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @return double time elapsed overall/since last reset
 */
double scene_get_time(scene_t *scene);

/**
 * @brief reset the time parameter to 0.0
 *
 * @param scene a pointer to a scene returned from scene_init()
 */
void scene_reset_time(scene_t *scene);

/**
 * @brief gets the index of a given body in the scene
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param body a pointer to a body
 *
 * @returns the index of the body in the scene
 */
int scene_get_index(scene_t *scene, body_t *body);

/**
 * @brief gets the sound_set of the scene
 *
 * @param scene a pointer to a scene returned from scene_init()
 *
 * @returns the sound_set of the scene
 */
sound_set_t *scene_get_sound_set(scene_t *scene);

/**
 * @brief retruns true if all bodies in the scene have 0 velocity
 */
bool scene_is_still(scene_t *scene);

/**
 * Toggle the boolean muted field of a scene and the music playing
 *
 * @param scene a pointer to a scene returned from scene_init()
 */
void scene_toggle_muted(scene_t *scene);

Mix_Chunk *scene_get_ball_ball(scene_t *scene);
Mix_Chunk *scene_get_cue_ball(scene_t *scene);
Mix_Chunk *scene_get_pocket_ball(scene_t *scene);
Mix_Chunk *scene_get_wall_ball(scene_t *scene);

#endif // #ifndef __SCENE_H__
