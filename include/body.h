#ifndef __BODY_H__
#define __BODY_H__

#include "color.h"
#include "list.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

/**
 * A rigid body constrained to the plane.
 * Implemented as a polygon with uniform density.
 * Bodies can accumulate forces and impulses during each tick.
 * Angular physics (i.e. torques) are not currently implemented.
 */
typedef struct body body_t;

/**
 * Initializes a body without any info.
 * Acts like body_init_with_info() where info and info_freer are NULL.
 */
body_t *body_init(list_t *shape, double mass, rgb_color_t color);

/**
 * Initializes a body without a sprite attached.
 * Acts like body_init_with_info_and_sprite() where info, image_path, and
 * info_freer are NULL.
 */
body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color,
                            void *info, free_func_t info_freer);

/**
 * Allocates memory for a body with the given parameters.
 * The body is initially at rest.
 * Asserts that the mass is positive and that the required memory is allocated.
 *
 * @param shape a list of vectors describing the initial shape of the body
 * @param mass the mass of the body (if INFINITY, stops the body from moving)
 * @param color the color of the body, used to draw it on the screen
 * @param info additional information to associate with the body,
 *   e.g. its type if the scene has multiple types of bodies
 * @param image_path image path of the sprite attached to the body
 * @param info_freer if non-NULL, a function call on the info to free it
 * @return a pointer to the newly allocated body
 */
body_t *body_init_with_info_and_sprite(list_t *shape, double mass,
                                       rgb_color_t color, void *info,
                                       SDL_Texture *image_path,
                                       free_func_t info_freer);

/**
 * Releases the memory allocated for a body.
 *
 * @param body a pointer to a body returned from body_init()
 */
void body_free(body_t *body);

/**
 * Gets the current shape of a body.
 * Returns a newly allocated vector list, which must be list_free()d.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the polygon describing the body's current position
 */
list_t *body_get_shape(body_t *body);

/**
 * Gets the current angle of body
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's current angular position
 */
double body_get_angle(body_t *body);

/**
 * Gets the current center of mass of a body.
 * While this could be calculated with polygon_centroid(), that becomes too slow
 * when this function is called thousands of times every tick.
 * Instead, the body should store its current centroid.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's center of mass
 */
vector_t body_get_centroid(body_t *body);

/**
 * Gets the current center point of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's center of mass
 */
vector_t body_get_center(body_t *body);

/**
 * Gets the current velocity of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's velocity vector
 */
vector_t body_get_velocity(body_t *body);

/**
 * Gets the mass of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the mass passed to body_init(), which must be greater than 0
 */
double body_get_mass(body_t *body);

/**
 * Gets the display color of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the color passed to body_init(), as an (R, G, B) tuple
 */
rgb_color_t body_get_color(body_t *body);

/**
 * Gets the information associated with a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the info passed to body_init()
 */
void *body_get_info(body_t *body);

/**
 * Gets the image associated with a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the image if it exists, NULL if it does not
 */
SDL_Texture *body_get_image(body_t *body);

/**
 * Gets the image path associated with a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the shadow if it exists, NULL if it does not
 */
SDL_Texture *body_get_shadow(body_t *body);

/**
 * Gets the dimensions associated with a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the vector_t of dimensions if it exists, NULL if it does not
 */
vector_t body_get_dimensions(body_t *body);

/**
 * Translates a body to a new position.
 * The position is specified by the position of the body's center of mass.
 *
 * @param body a pointer to a body returned from body_init()
 * @param v the body's new centroid
 */
void body_set_centroid(body_t *body, vector_t v);

/**
 * Changes a body's velocity (the time-derivative of its position).
 *
 * @param body a pointer to a body returned from body_init()
 * @param v the body's new velocity
 */
void body_set_velocity(body_t *body, vector_t v);

/**
 * Changes the display color of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @param color the body's new color
 */
void body_set_color(body_t *body, rgb_color_t color);

/**
 * Changes a body's orientation in the plane.
 * The body is rotated about its center of mass.
 * Note that the angle is *relative* to its current orientation, not absolute.
 *
 * @param body a pointer to a body returned from body_init()
 * @param angle the body's rotation angle in radians. Positive is
 * counterclockwise.
 */
void body_rotate(body_t *body, double angle);

/**
 * Changes a body's orientation in the plane.
 * The body is rotated about its center of mass.
 * Note that the angle is *absolute*, not relative to the current orientation.
 *
 * @param body a pointer to a body returned from body_init()
 * @param angle the body's new angle in radians. Positive is counterclockwise.
 */
void body_set_rotation(body_t *body, double angle);

/**
 * Sets the dimensions of a sprite in the scene.
 *
 * @param body a pointer to a body returned from body_init()
 * @param dimensions the body's new dimensions.
 */
void body_set_dimensions(body_t *body, vector_t dimensions);

/**
 * Sets the image of a sprite in the scene.
 *
 * @param body a pointer to a body returned from body_init()
 * @param image the body's new sprite.
 */
void body_set_image(body_t *body, SDL_Texture *image);

/**
 * Sets the shadow of a sprite in the scene.
 *
 * @param body a pointer to a body returned from body_init()
 * @param shadow the body's new shadow sprite.
 */
void body_set_shadow(body_t *body, SDL_Texture *shadow);

/**
 * @brief Rotates a body about a point
 *
 * @param body a pointer to a body returned from body_init()
 * @param angle the body's new angle in radians. Positive is counterclockwise.
 * @param point point to rotate the polygon about
 */
void body_rotate_about_point(body_t *body, double angle, vector_t point);

/**
 * @brief Translates body from current point.
 *
 * @param body a pointer to a body returned from body_init()
 * @param displacement vector_t representing the distance to translate the body
 */
void body_translate(body_t *body, vector_t displacement);

/**
 * Applies a force to a body over the current tick.
 * If multiple forces are applied in the same tick, they should be added.
 * Should not change the body's position or velocity; see body_tick().
 *
 * @param body a pointer to a body returned from body_init()
 * @param force the force vector to apply
 */
void body_add_force(body_t *body, vector_t force);

/**
 * Applies an impulse to a body.
 * An impulse causes an instantaneous change in velocity,
 * which is useful for modeling collisions.
 * If multiple impulses are applied in the same tick, they should be added.
 * Should not change the body's position or velocity; see body_tick().
 *
 * @param body a pointer to a body returned from body_init()
 * @param impulse the impulse vector to apply
 */
void body_add_impulse(body_t *body, vector_t impulse);

/**
 * Updates the body after a given time interval has elapsed.
 * Sets acceleration and velocity according to the forces and impulses
 * applied to the body during the tick.
 * The body should be translated at the *average* of the velocities before
 * and after the tick.
 * Resets the forces and impulses accumulated on the body.
 *
 * @param body the body to tick
 * @param dt the number of seconds elapsed since the last tick
 */
void body_tick(body_t *body, double dt);

/**
 * Marks a body for removal--future calls to body_is_removed() will return
 * true. Does not free the body. If the body is already marked for removal,
 * does nothing.
 *
 * @param body the body to mark for removal
 */
void body_remove(body_t *body);

/**
 * Returns whether a body has been marked for removal.
 * This function returns false until body_remove() is called on the body,
 * and returns true afterwards.
 *
 * @param body the body to check
 * @return whether body_remove() has been called on the body
 */
bool body_is_removed(body_t *body);

/**
 * Dilates the internal list of vectors for a body in the x
 * direction by a given amount
 *
 * @param body body to dilate
 * @param factor amount to stretch each vector by
 */
void body_stretch_x(body_t *body, double factor);

/**
 * Marks a body to be respawned instead of removed
 *
 * @param body body to set for respawning
 * @param respawnable the boolean
 */
void body_set_respawnable(body_t *body, bool respawnable);

/**
 * Returns whether a body will be respawned instead of removed
 *
 * @param body body in question
 * @return whether it is set to be respawned
 */
bool body_get_respawnable(body_t *body);

/**
 * Marks a body to be respawned the next tick
 *
 * @param body body to set for respawning
 * @param respawnable the boolean
 */
void body_set_to_respawn(body_t *body, bool to_respawn);

/**
 * Returns whether a body will be respawned on the next tick
 *
 * @param body body in question
 * @return whether it should respawn on the next tick
 */
bool body_to_respawn(body_t *body);

/**
 * Marks a body to be hidden or not
 *
 * @param body body to set as hidden or visible
 * @param respawnable the boolean
 */
void body_hide(body_t *body, bool hidden);

/**
 * Returns whether a body is hidden
 *
 * @param body body in question
 * @return whether it is hidden
 */
bool body_hidden(body_t *body);

/**
 * Marks a body for whether it should respawn on the next turn.
 * Note that this does not indicate where it should respawn (that
 * is given by body_set_for_player_respawn)
 *
 * @param body body to mark
 */
void body_respawn(body_t *body);

void body_set_apply_forces(body_t *body, bool apply_forces);

bool body_get_apply_forces(body_t *body);

void body_set_alpha(body_t *body, double alpha);

double body_get_alpha(body_t *body);

#endif // #ifndef __BODY_H__
