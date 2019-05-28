#include "body.h"

struct body {
    List *shape; // original shape...never gets modified
    Vector centroid;
    Vector velocity;
    double mass;
    RGBColor color;
    double angle; // absolute angle of the shape
    Vector force;
    Vector impulse;
    void *info;
    FreeFunc info_freer;
    bool is_removed;
    bool is_collided;
};

Body *body_init(List *shape, double mass, RGBColor color) {
    return body_init_with_info(shape, mass, color, NULL, NULL);
}

Body *body_init_with_info(List *shape, double mass, RGBColor color, void *info,
    FreeFunc info_freer) {
    assert(shape != NULL && mass > 0);
    Body *res = malloc(sizeof(Body));
    assert(res != NULL);
    res->centroid = polygon_centroid(shape);
    res->shape = shape;

    for (size_t i = 0; i < list_size(shape); i++) {
        Vector *tmp = list_get(shape, i);
        Vector new_vec = vec_subtract(*tmp, res->centroid);
        tmp->x = new_vec.x;
        tmp->y = new_vec.y;
    }

    res->velocity = (Vector) {0, 0};
    res->mass = mass;
    res->color = color;
    res->angle = 0;
    res->force = (Vector) {0, 0};
    res->impulse = (Vector) {0, 0};
    res->info = info;
    res->info_freer = info_freer;
    res->is_removed = false;
    res->is_collided = false;
    return res;
}

void body_free(Body *body) {
    assert(body != NULL);
    list_free(body->shape);

    if (body->info_freer != NULL && body->info != NULL) {
        body->info_freer(body->info);
    }

    free(body);
}

List *body_get_shape(Body *body) {
    assert(body != NULL);
    size_t size = list_size(body->shape);
    List *res = list_init(size, (FreeFunc) vec_free);
    assert(res != NULL);
    size_t i;

    for (i = 0; i < size; i++) {
        Vector *tmp = (Vector *) list_get(body->shape, i);
        Vector new_vec = vec_add(vec_rotate(*tmp, body->angle), body->centroid);
        list_add(res, vec_init(new_vec.x, new_vec.y));
    }

    return res;
}

Vector body_get_centroid(Body *body) {
    assert(body != NULL);
    return body->centroid;
}

Vector body_get_velocity(Body *body) {
    assert(body != NULL);
    return body->velocity;
}

double body_get_mass(Body *body) {
    assert(body != NULL);
    return body->mass;
}

RGBColor body_get_color(Body *body) {
    assert(body != NULL);
    return body->color;
}

void *body_get_info(Body *body) {
    assert(body != NULL);
    return body->info;
}

void body_set_centroid(Body *body, Vector x) {
    assert(body != NULL);
    body->centroid = x;
}

void body_set_velocity(Body *body, Vector v) {
    assert(body != NULL);
    body->velocity = v;
}

void body_set_rotation(Body *body, double angle) {
    assert(body != NULL);
    body->angle = angle;
}

void body_add_force(Body *body, Vector force) {
    assert(body != NULL);

    if (body_get_mass(body) != INFINITY) {
        body->force = vec_add(body->force, force);
    }
}

void body_add_impulse(Body *body, Vector impulse) {
    assert(body != NULL);
    body->impulse = vec_add(body->impulse, impulse);
}

void body_tick(Body *body, double dt) {
    assert(body != NULL);

    if (dt == 0) {
        return;
    }

    Vector a   = vec_add(vec_multiply(1.0 / body_get_mass(body), body->force), vec_multiply((1.0 / dt) / body_get_mass(body), body->impulse));
    Vector dv  = vec_multiply(dt, a);
    Vector avg = vec_add(body_get_velocity(body), vec_multiply(0.5, dv));
    Vector dx  = vec_multiply(dt, avg);

    body_set_centroid(body, vec_add(body->centroid, dx));
    body_set_velocity(body, vec_add(body->velocity, dv));
    body->force = (Vector) {0, 0};
    body->impulse = (Vector) {0, 0};
}

void body_remove(Body *body) {
    assert(body != NULL);
    body->is_removed = true;
}

bool body_is_removed(Body *body) {
    assert(body != NULL);
    return body->is_removed;
}


void body_collided(Body *body, bool stat) {
    assert(body != NULL);
    body->is_collided = stat;
}

bool body_is_collided(Body *body) {
    assert(body != NULL);
    return body->is_collided;
}



