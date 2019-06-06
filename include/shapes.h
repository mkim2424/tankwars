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
    BULLET2
} BodyType;


typedef struct {
    BodyType b;
} Body_info;

Body *n_polygon_shape(size_t num_sides, double radius, double mass,
    RGBColor color, Vector centroid, BodyType bt);

Body *star_shape(size_t num_sides);

Body *pac_man_shape();

Body *enemy_shape(size_t num_sides, double radius, Vector position);

Body *rectangle_shape(Vector centroid, double mass, double width, double height, RGBColor color, BodyType b);

#endif // #ifndef __SHAPES_H__
