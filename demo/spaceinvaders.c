#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "forces.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "shapes.h"
#include "color.h"

#define WIDTH 4000
#define HEIGHT 2000
#define ROWS 3
#define COLUMNS 8
#define ENEMY_NUM_SIDES 50
#define ENEMY_RADIUS 150
#define SCREEN_OFFSET 500
#define VERTICAL_OFFSET 10
#define PLAYER_HEIGHT 50
#define PLAYER_WIDTH 200
#define PROJECTILE_HEIGHT 50
#define PROJECTILE_WIDTH 10
#define MASS 10

const Vector min = {.x = 0, .y = 0};
const Vector max = {.x = WIDTH, .y = HEIGHT};
const Vector enemy_right_velocity = {.x = 500, .y = 0};
const Vector enemy_left_velocity = {.x = -500, .y = 0};
const Vector player_right_velocity = {.x = 1000, .y = 0};
const Vector player_left_velocity = {.x = -1000, .y = 0};
const Vector projectile_velocity = {.x = 0, .y = 1000};

/*
 * Helper function to get the nth bodytype in a scene.
 *
 * @param scene the scene of bodies and force creators.
 * @parma n     the index of the desired body.
 */
BodyType get_nth_bodytype(Scene *scene, size_t n) {
    Body_info *tmp = body_get_info(scene_get_body(scene, n));
    return (*tmp).b;
}

/*
 * Helper function to determine if the game is over by checking that the player
 * still exists.
 *
 * @param scene the scene of bodies and force creators.
 */
bool player_exists(Scene *scene) {
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        if (get_nth_bodytype(scene, i) == PLAYER) {
            return true;
        }
    }

    return false;
}

// Checks if enemies exist
bool enemy_exists(Scene *scene) {
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        if (get_nth_bodytype(scene, i) == ENEMY) {
            return true;
        }
    }

    return false;
}

void create_force(Scene *scene, Body *projectile) {
    Body *enemy;

    for (int i = 0; i < scene_bodies(scene); i++) {
        if (get_nth_bodytype(scene, i) == ENEMY) {
            enemy = scene_get_body(scene, i);
            create_destructive_collision(scene, enemy, projectile);
        }
    }
}

// Movement and actions for different keys
void on_key(char key, KeyEventType type, double held_time, void *aux) {
    Scene *scene = (Scene *) aux;
    Body *player, *projectile;

    for (int i = 0; i < scene_bodies(scene); i++) {
        if (get_nth_bodytype(scene, i) == PLAYER) {
            player = scene_get_body(scene, i);
            break;
        }
    }

    if (type == KEY_PRESSED) {
        switch (key) {
            case RIGHT_ARROW:
                //body_set_centroid(player, vec_add(body_get_centroid(player), vec_multiply(held_time, player_right_velocity)));
                body_set_velocity(player, player_right_velocity);
                break;

            case LEFT_ARROW:
                //body_set_centroid(player, vec_add(body_get_centroid(player), vec_multiply(held_time, player_left_velocity)));
                body_set_velocity(player, player_left_velocity);
                break;

            case ' ':
                projectile = rectangle_shape((Vector) {.x = body_get_centroid(player).x, .y = 2 * PLAYER_HEIGHT}, MASS,
                    PROJECTILE_WIDTH, PROJECTILE_HEIGHT, (RGBColor) {.r = 0, .g = 1, .b = 0}, PROJECTILE);
                body_set_velocity(projectile, projectile_velocity);
                scene_add_body(scene, projectile);
                create_force(scene, projectile);
                break;
        }
    } else if (type == KEY_RELEASED) {
        body_set_velocity(player, (Vector) {.x = 0, .y = 0});
    }
}

// Checks if player has crossed any boundaries
void check_player_boundary(Body *player) {
    if (body_get_centroid(player).x < PLAYER_WIDTH) {
        body_set_centroid(player,
            (Vector) {.x = PLAYER_WIDTH, .y = PLAYER_HEIGHT});
        body_set_velocity(player, min);
    }

    if (body_get_centroid(player).x > WIDTH - PLAYER_WIDTH) {
        body_set_centroid(player,
            (Vector) {.x = WIDTH - PLAYER_WIDTH, .y = PLAYER_HEIGHT});
        body_set_velocity(player, min);
    }
}

// Checks if enemy has crossed any boundaries
void check_enemy_boundary(Body *enemy) {
    Vector shift = {.x = 0, .y = 3 * (ENEMY_RADIUS + VERTICAL_OFFSET) + VERTICAL_OFFSET};

    if (body_get_centroid(enemy).x > WIDTH - ENEMY_RADIUS
            && body_get_velocity(enemy).x > 0) {
        body_set_centroid(enemy, vec_subtract(body_get_centroid(enemy), shift));
        body_set_velocity(enemy, enemy_left_velocity);
    }

    if (body_get_centroid(enemy).x < ENEMY_RADIUS
            && body_get_velocity(enemy).x < 0) {
        body_set_centroid(enemy, vec_subtract(body_get_centroid(enemy), shift));
        body_set_velocity(enemy, enemy_right_velocity);
    }
}

// Checks if projectile has crossed any boundaries
void check_projectile_boundary(Scene *scene) {
    Body *projectile;
    for (int i = 0; i < scene_bodies(scene); i++) {
        if ((* (Body_info *) body_get_info(scene_get_body(scene, i))).b == PROJECTILE) {
            projectile = scene_get_body(scene, i);
            if (body_get_centroid(projectile).y > HEIGHT + PROJECTILE_HEIGHT
                    || body_get_centroid(projectile).y < -PROJECTILE_HEIGHT) {
                body_remove(projectile);
            }
        }
    }
}

// Check for crossing of boundaries
void check_boundary(Scene *scene) {
    Body *player, *enemy;

    for (int i = 0; i < scene_bodies(scene); i++) {
        if ((* (Body_info *) body_get_info(scene_get_body(scene, i))).b == PLAYER) {
            player = scene_get_body(scene, i);
            check_player_boundary(player);
        }
        else if ((* (Body_info *) body_get_info(scene_get_body(scene, i))).b == ENEMY) {
            enemy = scene_get_body(scene, i);
            check_enemy_boundary(enemy);
        }
    }
}

void add_enemy_projectile(Scene *scene, Body *player) {
    int count = 0;

    for (size_t i = 0; i < scene_bodies(scene); i++) {
        if ((* (Body_info *) body_get_info(scene_get_body(scene, i))).b == ENEMY) {
            count += 1;
        }
    }

    int random_enemy = rand() % count;
    int temp = 0;

    for (int i = 0; i < scene_bodies(scene); i++) {
        if ((* (Body_info *) body_get_info(scene_get_body(scene, i))).b == ENEMY) {
            if (temp == random_enemy) {
                Body *projectile = rectangle_shape(body_get_centroid(scene_get_body(scene, i)), MASS,
                    PROJECTILE_WIDTH, PROJECTILE_HEIGHT, (RGBColor) {.r = 1, .g = 0, .b = 0}, PROJECTILE);
                body_set_velocity(projectile, vec_multiply(-1, projectile_velocity));
                scene_add_body(scene, projectile);
                create_destructive_collision(scene, projectile, player);
            }

            temp++;
        }
    }
}

int main(int argc, char *argv[]) {
    Scene *scene = scene_init();
    sdl_init(min, max);
    sdl_on_key(on_key, scene);
    size_t i, j;
    double dt;
    double enemy_radius = ENEMY_RADIUS;
    double offset = (WIDTH - (enemy_radius * COLUMNS * 2)) / (COLUMNS + 1);

    // Make player
    Body *player = rectangle_shape((Vector) {.x = WIDTH/2, .y = PLAYER_HEIGHT}, MASS,
        PLAYER_WIDTH, PLAYER_HEIGHT, (RGBColor) {.r = 0, .g = 0, .b = 0}, PLAYER);
    scene_add_body(scene, player);

    // Make enemies
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLUMNS; j++) {
            Body *enemy = enemy_shape(ENEMY_NUM_SIDES, enemy_radius,
                (Vector) {.x = (offset + enemy_radius) * (j+2),
                            .y = HEIGHT - (VERTICAL_OFFSET + enemy_radius) * (i+1)});
            body_set_velocity(enemy, enemy_right_velocity);
            scene_add_body(scene, enemy);
            create_destructive_collision(scene, player, enemy);
        }
    }

    double counter = 0;

    while (!sdl_is_done()) {
        dt = time_since_last_tick();
        counter += dt;

        if (!enemy_exists(scene)) {
            puts("Enemy no longer exists...ending game.");
            break;
        }

        if (counter > 0.5) {
            add_enemy_projectile(scene, player);
            check_projectile_boundary(scene);
            counter = 0;
        }

        if (!player_exists(scene)) {
            puts("Player no longer exists...ending game.");
            break;
        }

        scene_tick(scene, dt);
        sdl_render_scene(scene);
        check_boundary(scene);
    }
    return 0;
}
