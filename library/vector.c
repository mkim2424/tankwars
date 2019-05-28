#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

const Vector VEC_ZERO = {0, 0};

Vector *vec_init(double x, double y) {
    Vector *res = malloc(sizeof(Vector));
    res->x = x;
    res->y = y;
    return res;
}

void vec_free(Vector *vec) {
    free(vec);
}

double vec_magnitude(Vector v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

Vector vec_add(Vector v1, Vector v2) {
    Vector res;
    res.x = v1.x + v2.x;
    res.y = v1.y + v2.y;
    return res;
}

Vector vec_subtract(Vector v1, Vector v2) {
    Vector res;
    res.x = v1.x - v2.x;
    res.y = v1.y - v2.y;
    return res;
}

Vector vec_negate(Vector v) {
    return vec_multiply(-1, v);
}

Vector vec_multiply(double scalar, Vector v) {
    Vector res;
    res.x = v.x * scalar;
    res.y = v.y * scalar;
    return res;
}

double vec_dot(Vector v1, Vector v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

double vec_cross(Vector v1, Vector v2) {
    return v1.x * v2.y - v1.y * v2.x;
}

Vector vec_rotate(Vector v, double angle) {
    // Rotation matrix given by R(angle) = [cos(angle)  -sin(angle)]
    //                                   = [sin(angle)   cos(angle)]

    Vector res;
    res.x = v.x * cos(angle) - v.y * sin(angle);
    res.y = v.x * sin(angle) + v.y * cos(angle);
    return res;
}

Vector vec_norm(Vector v) {
    return (Vector) {.x = v.y, .y = -v.x};
}

Vector vec_normalize(Vector v) {
    double length = sqrt(v.x * v.x + v.y * v.y);
    return (Vector) {.x = v.x / length, .y = v.y / length};
}
