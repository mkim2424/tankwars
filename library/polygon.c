#include "polygon.h"

double polygon_area(List *polygon) {
    size_t size = list_size(polygon);
    double area = 0;
    size_t i;

    for (i = 0; i < size - 1; i++) {
        Vector *curr = (Vector *) list_get(polygon, i);
        Vector *next = (Vector *) list_get(polygon, i + 1);

        area += curr->x * next->y - next->x * curr->y;
    }

    Vector *first = (Vector *) list_get(polygon, 0);
    Vector *last = (Vector *) list_get(polygon, size - 1);

    area += last->x * first->y - first->x * last->y;
    return area / 2;
}

Vector polygon_centroid(List *polygon) {
    size_t size = list_size(polygon);
    double area = polygon_area(polygon);
    double tmp;
    size_t i;
    Vector res = {.x = 0, .y = 0};

    for (i = 0; i < size - 1; i++) {
        Vector *curr = (Vector *) list_get(polygon, i);
        Vector *next = (Vector *) list_get(polygon, i + 1);

        tmp = curr->x * next->y - curr->y * next->x;
        res.x += (curr->x + next->x) * tmp;
        res.y += (curr->y + next->y) * tmp;
    }

    Vector *first = (Vector *) list_get(polygon, 0);
    Vector *last = (Vector *) list_get(polygon, size - 1);
    tmp = last->x * first->y - last->y * first->x;
    res.x += (last->x + first->x) * tmp;
    res.y += (last->y + first->y) * tmp;
    res.x /= 6 * area;
    res.y /= 6 * area;
    return res;
}

void polygon_translate(List *polygon, Vector translation) {
    size_t i;
    Vector *tmp;
    Vector new_vec;

    for (i = 0; i < list_size(polygon); i++) {
        tmp = (Vector *) list_get(polygon, i);
        new_vec = vec_add(*tmp, translation);
        tmp->x = new_vec.x;
        tmp->y = new_vec.y;
    }
}

void polygon_rotate(List *polygon, double angle, Vector point) {
    size_t i;
    Vector *tmp;
    Vector new_vec;

    for (i = 0; i < list_size(polygon); i++) {
        tmp = (Vector *) list_get(polygon, i);
        new_vec = vec_add(vec_rotate(vec_subtract(*tmp, point), angle), point);
        tmp->x = new_vec.x;
        tmp->y = new_vec.y;
    }
}
