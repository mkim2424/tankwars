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
#define TANK_MASS 10
#define TANK_WIDTH 200
#define TANK_HEIGHT 125
#define WALL_LENGTH 150
#define PLAYER_SPEED 2500
#define ROWS 3
#define COLUMNS 10
#define OFFSET 10
#define BOX_OFFSET 20
#define BALL_NUM_SIDES 50
#define ENEMY_RADIUS 150
#define SCREEN_OFFSET 500
#define VERTICAL_OFFSET 10
#define PLAYER_HEIGHT 50
#define PLAYER_WIDTH 200
#define INFINITE_MASS 10000000

const Vector min = {.x = 0, .y = 0};
const Vector max = {.x = WIDTH, .y = HEIGHT};
const Vector player_start_velocity = {.x = 0, .y = 0};
const Vector player_up_velocity = {.x = 0, .y = PLAYER_SPEED};
const Vector player_down_velocity = {.x = 0, .y = -PLAYER_SPEED};
const Vector player_right_velocity = {.x = PLAYER_SPEED, .y = 0};
const Vector player_left_velocity = {.x = -PLAYER_SPEED, .y = 0};
const Vector ball_velocity = {.x = 1000, .y = 2000};

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

void set_rotation(Body *p) {
    Vector vel = body_get_velocity(p);
    if (vel.x != 0 || vel.y != 0) {
        if (vel.x == vel.y) {
            body_set_rotation(p, M_PI/4);
        }
        else if (vel.x == -vel.y) {
            body_set_rotation(p, 3*M_PI/4);
        }
        else if (vel.x == 0) {
            body_set_rotation(p, M_PI/2);
        }
        else {
            body_set_rotation(p, 0);
        }
    }
}

// Movement and actions for different keys
void on_key(char key, KeyEventType type, double held_time, void *aux) {
    Scene *scene = (Scene *) aux;
    Body *p1, *p2, *t1, *t2;
    Vector vel;

    for (int i = 0; i < scene_bodies(scene); i++) {
        if (get_nth_bodytype(scene, i) == ONE) {
            p1 = scene_get_body(scene, i);
        }
        if (get_nth_bodytype(scene, i) == TWO) {
            p2 = scene_get_body(scene, i);
        }
        if (get_nth_bodytype(scene, i) == TURRET_ONE) {
            t1 = scene_get_body(scene, i);
        }
        if (get_nth_bodytype(scene, i) == TURRET_TWO) {
            t2 = scene_get_body(scene, i);
        }
    }
    if (type == KEY_PRESSED) {
        switch (key) {
            case 119:
                vel = body_get_velocity(p1);
                vel.y = PLAYER_SPEED;
                body_set_velocity(p1, vel);
                body_set_velocity(t1, vel);
                set_rotation(p1);
                break;
            case 115:
                vel = body_get_velocity(p1);
                vel.y = -PLAYER_SPEED;
                body_set_velocity(p1, vel);
                body_set_velocity(t1, vel);
                set_rotation(p1);
                break;
            case 100:
                vel = body_get_velocity(p1);
                vel.x = PLAYER_SPEED;
                body_set_velocity(p1, vel);
                body_set_velocity(t1, vel);
                set_rotation(p1);
                break;
            case 97:
                vel = body_get_velocity(p1);
                vel.x = -PLAYER_SPEED;
                body_set_velocity(p1, vel);
                body_set_velocity(t1, vel);
                set_rotation(p1);
                break;
            case 114:
                body_set_rate(t1, 3);
                break;
            case 116:
                body_set_rate(t1, -3);
                break;
            case UP_ARROW:
                vel = body_get_velocity(p2);
                vel.y = PLAYER_SPEED;
                body_set_velocity(p2, vel);
                set_rotation(p2);
                break;
            case DOWN_ARROW:
                vel = body_get_velocity(p2);
                vel.y = -PLAYER_SPEED;
                body_set_velocity(p2, vel);
                set_rotation(p2);
                break;
            case RIGHT_ARROW:
                vel = body_get_velocity(p2);
                vel.x = PLAYER_SPEED;
                body_set_velocity(p2, vel);
                set_rotation(p2);
                break;
            case LEFT_ARROW:
                vel = body_get_velocity(p2);
                vel.x = -PLAYER_SPEED;
                body_set_velocity(p2, vel);
                set_rotation(p2);
                break;
            case 110:
                body_set_rate(t2, 3);
                break;
            case 109:
                body_set_rate(t2, -3);
                break;
        }
    } else if (type == KEY_RELEASED) {
        switch (key) {
            case 119:
            case 115:
                vel = body_get_velocity(p1);
                vel.y = 0;
                body_set_velocity(p1, vel);
                body_set_velocity(t1, vel);
                set_rotation(p1);
                break;
            case 100:
            case 97:
                vel = body_get_velocity(p1);
                vel.x = 0;
                body_set_velocity(p1, vel);
                body_set_velocity(t1, vel);
                set_rotation(p1);
                break;
            case UP_ARROW:
            case DOWN_ARROW:
                vel = body_get_velocity(p2);
                vel.y = 0;
                body_set_velocity(p2, vel);
                set_rotation(p2);
                break;
            case RIGHT_ARROW:
            case LEFT_ARROW:
                vel = body_get_velocity(p2);
                vel.x = 0;
                body_set_velocity(p2, vel);
                set_rotation(p2);
                break;
            case 114:
            case 116:
                body_set_rate(t1, 0);
                break;
            case 110:
            case 109:
                body_set_rate(t2, 0);
                break;
        }
    }
}

// Checks if player has crossed any boundaries
void check_player_boundary(Body *player) {
    if (body_get_centroid(player).x < PLAYER_WIDTH/2 + OFFSET) {
        body_set_centroid(player,
            (Vector) {.x = PLAYER_WIDTH/2 + OFFSET, .y = PLAYER_HEIGHT});
        body_set_velocity(player, min);
    }

    if (body_get_centroid(player).x > WIDTH - PLAYER_WIDTH/2 - OFFSET) {
        body_set_centroid(player,
            (Vector) {.x = WIDTH - PLAYER_WIDTH/2 - OFFSET, .y = PLAYER_HEIGHT});
        body_set_velocity(player, min);
    }
}

// Check for crossing of boundaries
void check_boundary(Scene *scene) {
    Body *player;
    for (int i = 0; i < scene_bodies(scene); i++) {
        if (get_nth_bodytype(scene, i) == PLAYER) {
            player = scene_get_body(scene, i);
            check_player_boundary(player);
        }
    }
}

// Draw the rows and columns of boxes
void draw_boxes(Scene *scene, Body *ball) {
    size_t i, j;
    double horizontal_size = (WIDTH - ((COLUMNS + 1) * BOX_OFFSET)) / (COLUMNS);
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLUMNS; j++) {
            RGBColor color = get_color(j);
            Body *box = rectangle_shape((Vector) {.x = (BOX_OFFSET + horizontal_size/2)
                + (BOX_OFFSET + horizontal_size) * j,
                .y = (HEIGHT - PLAYER_HEIGHT) - (BOX_OFFSET + PLAYER_HEIGHT) * i}, 1000000,
                horizontal_size, PLAYER_HEIGHT, color, BOX);
            scene_add_body(scene, box);
            create_physics_collision(scene, 1, ball, box);
            create_half_destructive_collision(scene, ball, box);
        }
    }
}

// Draw the ground
void draw_background(Scene *scene) {
    Body *b1 = rectangle_shape((Vector) {.x = WIDTH / 2, .y = HEIGHT / 2}, INFINITE_MASS,
        WIDTH, HEIGHT, (RGBColor) {.r = .93, .g = .875, .b = .7273}, BACKGROUND);
    scene_add_body(scene, b1);
}

// Draw the terrain walls
void draw_walls(Scene *scene) {
    for (size_t i = 0; i < 8; i++) {
        Body *b1 = rectangle_shape((Vector) {.x = WIDTH / 2, .y = (HEIGHT / 2) - (3.5 * WALL_LENGTH) + (i * WALL_LENGTH)}, INFINITE_MASS,
            WALL_LENGTH, WALL_LENGTH, (RGBColor) {.r = .52, .g = .52, .b = .52}, WALL);
        scene_add_body(scene, b1);
    }


}

void draw_tanks(Scene *scene) {
    Body *tank1 = rectangle_shape((Vector) {.x = WIDTH / 8,
        .y = HEIGHT / 2}, TANK_MASS, TANK_WIDTH, TANK_HEIGHT, (RGBColor) {.r = 0, .g = 0, .b = 1}, ONE);
    Body *tank2 = rectangle_shape((Vector) {.x = 7 * WIDTH / 8,
        .y = HEIGHT / 2}, TANK_MASS, TANK_WIDTH, TANK_HEIGHT, (RGBColor) {.r = 1, .g = 0, .b = 0}, TWO);
    Body *turret1 = rectangle_shape((Vector) {.x = WIDTH / 8 + 75,
        .y = HEIGHT / 2}, TANK_MASS, 200, 30, (RGBColor) {.r = 0, .g = 0, .b = .6}, TURRET_ONE);
    Body *turret2 = rectangle_shape((Vector) {.x = 7 * WIDTH / 8 - 75,
        .y = HEIGHT / 2}, TANK_MASS, 200, 30, (RGBColor) {.r = .6, .g = 0, .b = 0}, TURRET_TWO);
    body_set_velocity(tank1, player_start_velocity);
    body_set_velocity(tank2, player_start_velocity);
    body_set_velocity(turret1, player_start_velocity);
    body_set_velocity(turret2, player_start_velocity);
    scene_add_body(scene, tank1);
    scene_add_body(scene, tank2);
    scene_add_body(scene, turret1);
    scene_add_body(scene, turret2);
}

void update_turret(Scene *scene) {
    Body *p1, *p2, *t1, *t2;
    for (int i = 0; i < scene_bodies(scene); i++) {
        if (get_nth_bodytype(scene, i) == ONE) {
            p1 = scene_get_body(scene, i);
        }
        if (get_nth_bodytype(scene, i) == TURRET_ONE) {
            t1 = scene_get_body(scene, i);
        }
        if (get_nth_bodytype(scene, i) == TWO) {
            p2 = scene_get_body(scene, i);
        }
        if (get_nth_bodytype(scene, i) == TURRET_TWO) {
            t2 = scene_get_body(scene, i);
        }
    }
    double new_angle = body_get_angle(t1);
    body_set_centroid(t1, vec_add(body_get_centroid(p1), (Vector) {.x = cos(new_angle) * 75, .y = sin(new_angle) * 75}));
    new_angle = body_get_angle(t2);
    body_set_centroid(t2, vec_add(body_get_centroid(p2), (Vector) {.x = cos(new_angle) * -75, .y = sin(new_angle) * -75}));
}

// Start the game and return all scene components
Scene *create_game() {
    Scene *scene = scene_init();
    sdl_init(min, max);
    sdl_on_key(on_key, scene);
    draw_background(scene);
    draw_walls(scene);
    draw_tanks(scene);
    return scene;
}

int main(int argc, char *argv[]) {
    double dt;
    Scene *scene = create_game();
    while (!sdl_is_done()) {
        dt = time_since_last_tick();
        update_turret(scene);
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }
    return 0;
}
