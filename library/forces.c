#include "forces.h"

#define CLOSE 1

typedef struct {
    double constant;
    List *bodies;
} force_info;


typedef struct {
    List *bodies;
    void *aux_val;
    CollisionHandler handler;
    bool collided_before;
} collision_info;


void gravity_creator(force_info *aux) {
    assert(aux != NULL);
    double G = aux->constant;
    Body *body1 = (Body *) list_get(aux->bodies, 0);
    Body *body2 = (Body *) list_get(aux->bodies, 1);
    assert(G > 0 && body1 != NULL && body2 != NULL);
    double m1 = body_get_mass(body1);
    double m2 = body_get_mass(body2);
    Vector r  = vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
    double force = G * m1 * m2 / vec_dot(r, r);
    double magnitude = sqrt(vec_dot(r, r));
    Vector force_gravity = vec_multiply(force / magnitude, r);

    if (magnitude > CLOSE) {
        body_add_force(body1, force_gravity);
        body_add_force(body2, vec_negate(force_gravity));
    }
}

void create_newtonian_gravity(Scene *scene, double G, Body *body1, Body *body2) {
    assert(scene != NULL && G > 0 && body1 != NULL && body2 != NULL);
    force_info *aux = malloc(sizeof(force_info));
    assert(aux != NULL);
    aux->constant = G;

    List *bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);
    aux->bodies = bodies;

    scene_add_bodies_force_creator(scene, (ForceCreator) gravity_creator, aux,
        bodies, (FreeFunc) free);
}

void spring_creator(force_info *aux) {
    assert(aux != NULL);
    double k = aux->constant;
    Body *body1 = (Body *) list_get(aux->bodies, 0);
    Body *body2 = (Body *) list_get(aux->bodies, 1);
    assert(k > 0 && body1 != NULL && body2 != NULL);
    Vector x  = vec_subtract(body_get_centroid(body2), body_get_centroid(body1));

    body_add_force(body1, vec_multiply(k, x));
    body_add_force(body2, vec_multiply(-k, x));
}

void create_spring(Scene *scene, double k, Body *body1, Body *body2) {
    assert(scene != NULL && k >= 0 && body1 != NULL && body2 != NULL);
    force_info *aux = malloc(sizeof(force_info));
    assert(aux != NULL);
    aux->constant = k;

    List *bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);
    aux->bodies = bodies;

    scene_add_bodies_force_creator(scene, (ForceCreator) spring_creator, aux,
        bodies, (FreeFunc) free);
}

void drag_creator(force_info *aux) {
    assert(aux != NULL);
    double gamma = aux->constant;
    Body *body = (Body *) list_get(aux->bodies, 0);
    assert(gamma >= 0 && body != NULL);

    body_add_force(body, vec_multiply(-gamma, body_get_velocity(body)));
}

void create_drag(Scene *scene, double gamma, Body *body) {
    assert(scene != NULL && gamma >= 0 && body != NULL);
    force_info *aux = malloc(sizeof(force_info));
    assert(aux != NULL);
    aux->constant = gamma;

    List *bodies = list_init(1, NULL);
    list_add(bodies, body);
    aux->bodies = bodies;

    scene_add_bodies_force_creator(scene, (ForceCreator) drag_creator, aux,
        bodies, (FreeFunc) free);
}

void destruction_creator(force_info *aux) {
    assert(aux != NULL);
    Body *body1 = (Body *) list_get(aux->bodies, 0);
    Body *body2 = (Body *) list_get(aux->bodies, 1);
    List *shape1 = body_get_shape(body1);
    List *shape2 = body_get_shape(body2);

    if (find_collision(shape1, shape2).collided) {
        body_remove(body1);
        body_remove(body2);
    }
}


void create_destructive_collision(Scene *scene, Body *body1, Body *body2) {
    assert(scene != NULL && body1 != NULL && body2 != NULL);
    force_info *aux = malloc(sizeof(force_info));
    assert(aux != NULL);
    aux->constant = 0;

    List *bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);
    aux->bodies = bodies;

    scene_add_bodies_force_creator(scene, (ForceCreator) destruction_creator,
        aux, bodies, (FreeFunc) free);
}





void half_destruction_creator(force_info *aux) {
    assert(aux != NULL);
    Body *body1 = (Body *) list_get(aux->bodies, 0);
    Body *body2 = (Body *) list_get(aux->bodies, 1);
    List *shape1 = body_get_shape(body1);
    List *shape2 = body_get_shape(body2);

    if (find_collision(shape1, shape2).collided) {
        body_remove(body2);
    }
}


void create_half_destructive_collision(Scene *scene, Body *body1, Body *body2) {
    assert(scene != NULL && body1 != NULL && body2 != NULL);
    force_info *aux = malloc(sizeof(force_info));
    assert(aux != NULL);
    aux->constant = 0;

    List *bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);
    aux->bodies = bodies;

    scene_add_bodies_force_creator(scene, (ForceCreator) half_destruction_creator,
        aux, bodies, (FreeFunc) free);
}


void collision_creator(collision_info *aux) {
    assert(aux != NULL);
    Body *body1 = (Body *) list_get(aux->bodies, 0);
    Body *body2 = (Body *) list_get(aux->bodies, 1);
    List *shape1 = body_get_shape(body1);
    List *shape2 = body_get_shape(body2);

    CollisionInfo collision = find_collision(shape1, shape2);
    if (collision.collided) {
        if (!aux->collided_before) {

            // take care of three bullet rule
            Body_info *body1_info = body_get_info(body1);
            Body_info *body2_info = body_get_info(body2);
            if ((*body1_info).b == BULLET1 || (*body1_info).b == BULLET2) {
                if (get_num_collided(body1) >= 3) {
                    body_remove(body1);
                }

                else {
                    increment_num_collided(body1);
                }
            } 
            if ((*body2_info).b == BULLET1 || (*body2_info).b == BULLET2) {
                if (get_num_collided(body2) >= 3) {
                    body_remove(body2);
                }

                else {
                    increment_num_collided(body2);
                }
            }

            // take care of WALL_BREAK only withholding 3 hits
            if ((*body1_info).b == WALL_BREAK && ((*body2_info).b == BULLET1 || (*body2_info).b == BULLET2)) {
                if (get_num_collided(body1) >= 2) {
                    body_remove(body1);
                }

                else {
                    increment_num_collided(body1);
                    RGBColor color = body_get_color(body1);
                    RGBColor new_color = {.r = color.r, .g = color.g + 0.15, .b = color.b};
                    body_set_color(body1, new_color);
                }
            }

            if ((*body2_info).b == WALL_BREAK && ((*body1_info).b == BULLET1 || (*body1_info).b == BULLET2)) {
                if (get_num_collided(body2) >= 2) {
                    body_remove(body2);
                }

                else {
                    increment_num_collided(body2);
                    RGBColor color = body_get_color(body2);
                    RGBColor new_color = {.r = color.r, .g = color.g + 0.15, .b = color.b};
                    body_set_color(body2, new_color);
                }
            }

            aux->collided_before = true;
            aux->handler(body1, body2, collision.axis, aux->aux_val);
        }
    }

    else {
        aux->collided_before = false;
    }

}


void create_collision(Scene *scene, Body *body1, Body *body2, CollisionHandler
 handler, void *aux, FreeFunc freer) {
    collision_info *aux1 = malloc(sizeof(collision_info));
    List *bodies = list_init(2, NULL);
    list_add(bodies, body1);
    list_add(bodies, body2);
    aux1->bodies = bodies;
    aux1->aux_val = aux;
    aux1->handler = handler;
    aux1->collided_before = false;

    scene_add_bodies_force_creator(scene, (ForceCreator) collision_creator,
        aux1, bodies, freer);

}




void physics_collision_handler(Body *body1, Body *body2, Vector axis, void *aux) {
    double *temp = (double *) aux;
    double C = *temp;
    double m1 = body_get_mass(body1);
    double m2 = body_get_mass(body2);
    double reduced_mass;
    if (m1 == INFINITY) {
        reduced_mass = m2;
    }
    else if (m2 == INFINITY) {
        reduced_mass = m1;
    }
    else {
        reduced_mass = (m1 * m2/(m1 + m2));
    }
    double u_a = vec_dot(body_get_velocity(body1), axis);
    double u_b = vec_dot(body_get_velocity(body2), axis);
    double j = reduced_mass * (1 + C) * (u_b - u_a);
    body_add_impulse(body1, vec_multiply(j, axis));
    body_add_impulse(body2, vec_multiply(-j, axis));

}


void create_physics_collision( Scene *scene, double elasticity, Body *body1,
                                     Body *body2)

{
    assert(scene != NULL && body1 != NULL && body2 != NULL);
    double *aux = malloc(sizeof(double));
    assert(aux != NULL);
    *aux = elasticity;

    create_collision(scene, body1, body2, 
        (CollisionHandler) physics_collision_handler, aux, (FreeFunc) free);

}
