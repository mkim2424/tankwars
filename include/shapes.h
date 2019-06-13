#ifndef __SHAPES_H__
#define __SHAPES_H__

#include "color.h"
#include "list.h"
#include "body.h"

/* This enumerates the different bodies */
typedef enum {
    PLAYER,
    ENEMY,
    PROJECTILE,
    BACKGROUND,
    BOX,
    BALL,
    ONE,
    TWO,
    TURRET_ONE,
    TURRET_TWO,
    WALL,
    WALL_BREAK,
    BULLET1,
    BULLET2,
    EXPLOSION
} BodyType;


typedef struct {
    BodyType b;
} Body_info;

/**
 * Creates and returns pointer to body of n sides
 *
 * @param num_sides number of sides
 * @param radius 
 * @param mass 
 * @param color 
 * @param centroid
 * @param bt BodyType of body
 */
Body *n_polygon_shape(size_t num_sides, double radius, double mass,
    RGBColor color, Vector centroid, BodyType bt);

/**
 * Creates and returns a pointer to star of n sides
 *
 * @param num_sides number of sides
 * @param radius 
 * @param mass 
 * @param color 
 * @param centroid
 * @param bt BodyType of body
 */
Body *star_shape(size_t num_sides, double radius, double mass,
    RGBColor color, Vector centroid, BodyType bt);

/**
 * Creates and returns a pointer to pacman.
 */
Body *pac_man_shape();


/**
* Creates and returns pointer to enemy in space invaders.
*
* @param num_sides number of sides
* @param radius 
* @param position
*/
Body *enemy_shape(size_t num_sides, double radius, Vector position);


/**
* Creates and returns pointer to a rectangle shape.
*
* @param centroid
* @param mass 
* @param width
* @param height
* @param color
* @param bt BodyType
*/
Body *rectangle_shape(Vector centroid, double mass, double width, double height, RGBColor color, BodyType b);

#endif // #ifndef __SHAPES_H__
