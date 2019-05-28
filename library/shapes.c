#include <math.h>
#include "shapes.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "polygon.h"


Body *n_polygon_shape(size_t num_sides, double radius, double mass,
    RGBColor color, Vector centroid) {
    List *vertices = list_init(num_sides, (FreeFunc) vec_free);
    double theta = 2 * M_PI / num_sides;
    Vector start = (Vector) {.x = radius, .y = 0};
    size_t i;

    for (i = 0; i < num_sides; i++) {
        Vector tmp = vec_rotate(start, theta * i);
        list_add(vertices, vec_init(tmp.x, tmp.y));
    }
    Body_info *body_i = malloc(sizeof(Body_info));
    body_i->b = BALL;

    Body *res = body_init_with_info(vertices, mass, color, body_i, free);
    body_set_centroid(res, centroid);
    return res;
}

Body *enemy_shape(size_t num_sides, double radius, Vector position) {
    List *vertices = list_init(num_sides, (FreeFunc) vec_free);

    double angle = M_PI / 6;
    Vector origin = (Vector) {.x = 0, .y = 0};
    Vector start = vec_rotate ((Vector) {.x = radius, .y = 0}, angle);
    list_add(vertices, vec_init(origin.x, origin.y));
    list_add(vertices, vec_init(start.x, start.y));

    size_t remaining_sides = num_sides - 2;
    // Angle of rotation between each point on polygon
    double angle_change = 2 * M_PI / (3 * remaining_sides);
    size_t i;
    for (i = 2; i < remaining_sides; i++) {
        Vector point = vec_rotate(*((Vector *) list_get(vertices, i-1)), angle_change);
        list_add(vertices, vec_init(point.x, point.y));
    }
    Body_info *body_i = malloc(sizeof(Body_info));
    body_i->b = ENEMY;

    // Mass is irrelevant
    Body *res = body_init_with_info(vertices, 10, (RGBColor) {.r = 1, .g = 0, .b = 0}, body_i, free);
    body_set_centroid(res, position);
    return res;
}

Body *rectangle_shape(Vector centroid, double mass, double width, double height, RGBColor color, BodyType bt) {
    List *vertices = list_init(4, (FreeFunc) vec_free);
    double d_x = width / 2;
    double d_y = height / 2;
    list_add(vertices, vec_init(centroid.x - d_x, centroid.y + d_y));
    list_add(vertices, vec_init(centroid.x - d_x, centroid.y - d_y));
    list_add(vertices, vec_init(centroid.x + d_x, centroid.y - d_y));
    list_add(vertices, vec_init(centroid.x + d_x, centroid.y + d_y));
    // Mass is irrelevant

    Body_info *body_i = malloc(sizeof(Body_info));
    body_i->b = bt;

    Body *res = body_init_with_info(vertices, mass, color, body_i, free);
    body_set_centroid(res, centroid);
    return res;
}
