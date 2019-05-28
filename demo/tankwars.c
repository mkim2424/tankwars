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
#define TANK_HEIGHT 150
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
#define BALL_RADIUS 10
#define BALL_MASS 10
#define INFINITE_MASS 10000000

const Vector min = {.x = 0, .y = 0};
const Vector max = {.x = WIDTH, .y = HEIGHT};
const Vector player_right_velocity = {.x = 2000, .y = 0};
const Vector player_left_velocity = {.x = -2000, .y = 0};
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


// Movement and actions for different keys
void on_key(char key, KeyEventType type, double held_time, void *aux) {
    Scene *scene = (Scene *) aux;
    Body *player;

    for (int i = 0; i < scene_bodies(scene); i++) {
        if (get_nth_bodytype(scene, i) == PLAYER) {
            player = scene_get_body(scene, i);
            break;
        }
    }
    if (type == KEY_PRESSED) {
        switch (key) {
            case RIGHT_ARROW:
                body_set_velocity(player, player_right_velocity);
                break;
            case LEFT_ARROW:
                body_set_velocity(player, player_left_velocity);
                break;
        }
    } else if (type == KEY_RELEASED) {
        body_set_velocity(player, (Vector) {.x = 0, .y = 0});
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

// Draw the boundary walls
void draw_background(Scene *scene) {
    Body *b1 = rectangle_shape((Vector) {.x = WIDTH / 2, .y = HEIGHT / 2}, INFINITE_MASS,
        WIDTH, HEIGHT, (RGBColor) {.r = .8671, .g = .7187, .b = .5273}, BACKGROUND);
    scene_add_body(scene, b1);
}

// Draw the player
void draw_player(Scene *scene, Body *ball) {
    Body *player = rectangle_shape((Vector) {.x = WIDTH/2, .y = PLAYER_HEIGHT}, INFINITE_MASS,
        PLAYER_WIDTH, PLAYER_HEIGHT, (RGBColor) {.r = 1, .g = 0, .b = 0}, PLAYER);
    scene_add_body(scene, player);
    create_physics_collision(scene, 1, ball, player);
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

// Start the game and return all scene components
Scene *create_game() {
    Scene *scene = scene_init();
    sdl_init(min, max);
    sdl_on_key(on_key, scene);
    draw_background(scene);
    Body *tank1 = rectangle_shape((Vector) {.x = WIDTH / 8,
        .y = HEIGHT / 2}, TANK_MASS, TANK_WIDTH, TANK_HEIGHT, (RGBColor) {.r = 0, .g = 0, .b = 1}, ONE);
    Body *tank2 = rectangle_shape((Vector) {.x = 7 * WIDTH / 8,
        .y = HEIGHT / 2}, TANK_MASS, TANK_WIDTH, TANK_HEIGHT, (RGBColor) {.r = 1, .g = 0, .b = 0}, TWO);
    scene_add_body(scene, tank1);
    scene_add_body(scene, tank2);
    return scene;
}

// Check if ball has gone off the screen
Scene *check_ball(Scene *scene) {
    for (int i = 0; i < scene_bodies(scene); i++) {
        if (get_nth_bodytype(scene, i) == BALL) {
            Body *ball = scene_get_body(scene, i);
            if (body_get_centroid(ball).y < 0) {
                return create_game();
            }
        }
    }
    return scene;
}

// Move row of boxes down
void move_row(Scene *scene) {
    for (int i = 0; i < scene_bodies(scene); i++) {
        if (get_nth_bodytype(scene, i) == BOX) {
            Body *box = scene_get_body(scene, i);
            body_set_centroid(box, vec_subtract(body_get_centroid(box),
                (Vector) {.x = 0, .y = BOX_OFFSET + PLAYER_HEIGHT}));
        }
    }
}

// Check if boxes have reached the bottom
bool check_box_collide_player(Scene *scene) {
    for (int i = 0; i < scene_bodies(scene); i++) {
        if (get_nth_bodytype(scene, i) == BOX) {
            Body *box = scene_get_body(scene, i);
            if (body_get_centroid(box).y < 2 * PLAYER_HEIGHT) {
                return true;
            }
        }
    }
    return false;
}

int main(int argc, char *argv[]) {
    double dt;
    Scene *scene = create_game();
    while (!sdl_is_done()) {
        dt = time_since_last_tick();
        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }
    return 0;
}
