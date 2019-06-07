#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL_Mixer.h>
#include "forces.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "shapes.h"
#include "color.h"
#include "body.h"
#include <time.h>

#define WIDTH 4000
#define HEIGHT 2000
#define TANK_MASS 10
#define TANK_WIDTH 200
#define TANK_HEIGHT 125
#define TURRET_WIDTH 200
#define TURRET_HEIGHT 30
#define WALL_LENGTH 150
#define PLAYER_SPEED 1000
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
#define BULLET_VELOCITY 1500
#define INFINITE_MASS INFINITY

const Vector min = {.x = 0, .y = 0};
const Vector max = {.x = WIDTH, .y = HEIGHT};
const Vector player_start_velocity = {.x = 0, .y = 0};
const RGBColor tank1_color = {.r = 0, .g = 0, .b = 1};
const RGBColor turret1_color = {.r = 0, .g = 0, .b = .6};
const RGBColor tank2_color = {.r = 1, .g = 0, .b = 0};
const RGBColor turret2_color = {.r = .6, .g = 0, .b = 0};
const RGBColor wall_color = {.r = .52, .g = .52, .b = .52};
const RGBColor wall_break_color = {.r = 1, .g = .35, .b = 0};
const RGBColor shrub_color = {.r = 0, .g = 0.7, .b = 0.3};


Mix_Music *music = NULL;
Mix_Chunk *shoot = NULL;
Mix_Chunk *boom = NULL;
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

void shoot_bullet1(Scene *scene, Body *tank1, Body *tank2, Body *turret) {
    double angle = body_get_angle(turret);
    Body *bullet = n_polygon_shape(20, 20, 10,
        turret1_color, vec_add(body_get_centroid(tank1), (Vector) {.x = cos(angle) * 200, .y = sin(angle) * 200}), BULLET1);
    body_set_velocity(bullet, (Vector) {.x = cos(angle) * BULLET_VELOCITY, .y = sin(angle) * BULLET_VELOCITY});
    // create physics collision between bullet and walls
    for (int i = 0; i < scene_bodies(scene); i++) {
        if (get_nth_bodytype(scene, i) == WALL || get_nth_bodytype(scene, i) == WALL_BREAK) {
            Body *wall = scene_get_body(scene, i);
            create_physics_collision(scene, 0.95, wall, bullet);
        }
    }

    // create destruction force between bullet and both tanks
    create_destructive_collision(scene, tank1, bullet);
    create_destructive_collision(scene, tank2, bullet);
    scene_add_body(scene, bullet);
}

void shoot_bullet2(Scene *scene, Body *tank2, Body *tank1, Body *turret) {
    double angle = body_get_angle(turret);
    Body *bullet = n_polygon_shape(20, 20, 10,
        turret2_color, vec_add(body_get_centroid(tank2), (Vector) {.x = cos(angle) * -200, .y = sin(angle) * -200}), BULLET2);
    body_set_velocity(bullet, (Vector) {.x = cos(angle) * -BULLET_VELOCITY, .y = sin(angle) * -BULLET_VELOCITY});
    // create physics collision between bullet and walls
    for (int i = 0; i < scene_bodies(scene); i++) {
        if (get_nth_bodytype(scene, i) == WALL || get_nth_bodytype(scene, i) == WALL_BREAK) {
            Body *wall = scene_get_body(scene, i);
            create_physics_collision(scene, 0.95, wall, bullet);
        }
    }

    // create destruction force between bullet and both tanks
    create_destructive_collision(scene, tank1, bullet);
    create_destructive_collision(scene, tank2, bullet);
    scene_add_body(scene, bullet);
}

int count_bullet1(Scene *scene) {
    int count = 0;
    for (int i = 0; i < scene_bodies(scene); i++) {
        if (get_nth_bodytype(scene, i) == BULLET1) {
            count += 1;
        }
    }
    return count;
}

int count_bullet2(Scene *scene) {
    int count = 0;
    for (int i = 0; i < scene_bodies(scene); i++) {
        if (get_nth_bodytype(scene, i) == BULLET2) {
            count += 1;
        }
    }
    return count;
}


// Movement and actions for different keys
void on_key(char key, KeyEventType type, double held_time, void *aux) {
    Scene *scene = (Scene *) aux;
    Body *p1, *p2, *t1, *t2;
    Vector vel;
    bool counter = true;

    for (int i = 0; i < scene_bodies(scene); i++) {
        if (get_nth_bodytype(scene, i) == EXPLOSION) {
            counter = false;
        }
    }

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
    if (counter) {
        if (type == KEY_PRESSED) {
            switch (key) {
                case 119:

                    if (p1 != NULL) {
                        vel = body_get_velocity(p1);
                        vel.y = PLAYER_SPEED;
                        body_set_velocity(p1, vel);
                        body_set_velocity(t1, vel);
                        set_rotation(p1);
                    }
                    break;
                case 115:
                    if (p1 != NULL) {
                        vel = body_get_velocity(p1);
                        vel.y = -PLAYER_SPEED;
                        body_set_velocity(p1, vel);
                        body_set_velocity(t1, vel);
                        set_rotation(p1);
                    }

                    break;
                case 100:
                    if (p1 != NULL) {
                        vel = body_get_velocity(p1);
                        vel.x = PLAYER_SPEED;
                        body_set_velocity(p1, vel);
                        body_set_velocity(t1, vel);
                        set_rotation(p1);

                    }

                    break;
                case 97:
                    if (p1 != NULL) {
                        vel = body_get_velocity(p1);
                        vel.x = -PLAYER_SPEED;
                        body_set_velocity(p1, vel);
                        body_set_velocity(t1, vel);
                        set_rotation(p1);

                    }

                    break;
                case 114:
                    body_set_rate(t1, 2);
                    break;
                case 116:
                    body_set_rate(t1, -2);
                    break;
                case 121:
                    if (count_bullet1(scene) < 5) {
                        Mix_PlayChannel( -1, shoot, 0 );
                        shoot_bullet1(scene, p1, p2, t1);
                    }
                    // printf("\a\n");
                    break;
                case UP_ARROW:
                    if (p2 != NULL) {
                        vel = body_get_velocity(p2);
                        vel.y = PLAYER_SPEED;
                        body_set_velocity(p2, vel);
                        set_rotation(p2);

                    }

                    break;
                case DOWN_ARROW:
                    if (p2 != NULL) {
                        vel = body_get_velocity(p2);
                        vel.y = -PLAYER_SPEED;
                        body_set_velocity(p2, vel);
                        set_rotation(p2);

                    }

                    break;
                case RIGHT_ARROW:
                    if (p2 != NULL) {
                        vel = body_get_velocity(p2);
                        vel.x = PLAYER_SPEED;
                        body_set_velocity(p2, vel);
                        set_rotation(p2);

                    }

                    break;
                case LEFT_ARROW:
                    if (p2 != NULL) {
                        vel = body_get_velocity(p2);
                        vel.x = -PLAYER_SPEED;
                        body_set_velocity(p2, vel);
                        set_rotation(p2);

                    }

                    break;
                case 110:
                    body_set_rate(t2, 2);
                    break;
                case 109:
                    body_set_rate(t2, -2);
                    break;
                case ' ':
                    if (count_bullet2(scene) < 5) {
                        Mix_PlayChannel( -1, shoot, 0 );
                        shoot_bullet2(scene, p2, p1, t2);
                    }
            }
        } else if (type == KEY_RELEASED) {
            switch (key) {
                case 119:
                case 115:
                    if (p1 != NULL) {
                        vel = body_get_velocity(p1);
                        vel.y = 0;
                        body_set_velocity(p1, vel);
                        body_set_velocity(t1, vel);
                        set_rotation(p1);

                    }

                    break;
                case 100:
                case 97:
                    if (p1 != NULL) {
                        vel = body_get_velocity(p1);
                        vel.x = 0;
                        body_set_velocity(p1, vel);
                        body_set_velocity(t1, vel);
                        set_rotation(p1);

                    }

                    break;
                case UP_ARROW:
                case DOWN_ARROW:
                    if (p2 != NULL) {
                        vel = body_get_velocity(p2);
                        vel.y = 0;
                        body_set_velocity(p2, vel);
                        set_rotation(p2);

                    }

                    break;
                case RIGHT_ARROW:
                case LEFT_ARROW:
                    if (p2 != NULL) {
                        vel = body_get_velocity(p2);
                        vel.x = 0;
                        body_set_velocity(p2, vel);
                        set_rotation(p2);
                    }

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

// Draw the ground
void draw_background(Scene *scene) {
    Body *b1 = rectangle_shape((Vector) {.x = WIDTH / 2, .y = HEIGHT / 2}, INFINITE_MASS,
        WIDTH, HEIGHT, (RGBColor) {.r = .93, .g = .875, .b = .7273}, BACKGROUND);
    scene_add_body(scene, b1);
}

// Draw the terrain walls
void draw_walls(Scene *scene) {

    RGBColor wall_c;
    BodyType wall_type;
    for (size_t i = 0; i < 8; i++) {
        if (i % 2 == 0) {
            wall_c = wall_color;
            wall_type = WALL;
        }

        else {
            wall_c = wall_break_color;
            wall_type = WALL_BREAK;
        }
        Body *b1 = rectangle_shape((Vector) {.x = WIDTH / 2, .y = (HEIGHT / 2) - (3.5 * WALL_LENGTH) + (i * (WALL_LENGTH + 10))}, INFINITE_MASS,
            WALL_LENGTH, WALL_LENGTH, wall_c, wall_type);
        scene_add_body(scene, b1);
    }


    // draw shrubs
    Body *shrub = rectangle_shape((Vector) {.x = 3250, .y = 1600}, INFINITE_MASS,
            WALL_LENGTH * 2, WALL_LENGTH * 2, shrub_color, WALL);
    Body *shrub1 = rectangle_shape((Vector) {.x = 3250, .y = 400}, INFINITE_MASS,
            WALL_LENGTH * 2, WALL_LENGTH * 2, shrub_color, WALL);
    Body *shrub2 = rectangle_shape((Vector) {.x = 700, .y = 1600}, INFINITE_MASS,
            WALL_LENGTH * 2, WALL_LENGTH * 2, shrub_color, WALL);
    Body *shrub3 = rectangle_shape((Vector) {.x = 700, .y = 400}, INFINITE_MASS,
            WALL_LENGTH * 2, WALL_LENGTH * 2, shrub_color, WALL);
    scene_add_body(scene, shrub);
    scene_add_body(scene, shrub1);
    scene_add_body(scene, shrub2);
    scene_add_body(scene, shrub3);
}

void draw_boundaries(Scene *scene) {
    Body *b1 = rectangle_shape((Vector) {.x = WIDTH/2, .y = HEIGHT}, INFINITE_MASS,
        WIDTH, 20, (RGBColor) {.r = 0, .g = 0, .b = 0}, WALL);
    Body *b2 = rectangle_shape((Vector) {.x = WIDTH/2, .y = 0}, INFINITE_MASS,
        WIDTH, 20, (RGBColor) {.r = 0, .g = 0, .b = 0}, WALL);
    Body *b3 = rectangle_shape((Vector) {.x = 0, .y = HEIGHT/2}, INFINITE_MASS,
        20, HEIGHT, (RGBColor) {.r = 0, .g = 0, .b = 0}, WALL);
    Body *b4 = rectangle_shape((Vector) {.x = WIDTH, .y = HEIGHT/2}, INFINITE_MASS,
        20, HEIGHT, (RGBColor) {.r = 0, .g = 0, .b = 0}, WALL);
    scene_add_body(scene, b1);
    scene_add_body(scene, b2);
    scene_add_body(scene, b3);
    scene_add_body(scene, b4);


}

void draw_tanks(Scene *scene) {
    Body *tank1 = rectangle_shape((Vector) {.x = WIDTH / 8,
        .y = HEIGHT / 2}, TANK_MASS, TANK_WIDTH, TANK_HEIGHT, tank1_color, ONE);
    Body *tank2 = rectangle_shape((Vector) {.x = 7 * WIDTH / 8,
        .y = HEIGHT / 2}, TANK_MASS, TANK_WIDTH, TANK_HEIGHT, tank2_color, TWO);
    Body *turret1 = rectangle_shape((Vector) {.x = WIDTH / 8 + 75,
        .y = HEIGHT / 2}, TANK_MASS, TURRET_WIDTH, TURRET_HEIGHT, turret1_color, TURRET_ONE);
    Body *turret2 = rectangle_shape((Vector) {.x = 7 * WIDTH / 8 - 75,
        .y = HEIGHT / 2}, TANK_MASS, TURRET_WIDTH, TURRET_HEIGHT, turret2_color, TURRET_TWO);
    body_set_velocity(tank1, player_start_velocity);
    body_set_velocity(tank2, player_start_velocity);
    body_set_velocity(turret1, player_start_velocity);
    body_set_velocity(turret2, player_start_velocity);
    // create physics collisions between two tanks
    create_physics_collision(scene, 0.5, tank1, tank2);

    // create physics collisions between walls and tanks
    for (int i = 0; i < scene_bodies(scene); i++) {
        if (get_nth_bodytype(scene, i) == WALL || get_nth_bodytype(scene, i) == WALL_BREAK) {
            Body *w = scene_get_body(scene, i);
            create_physics_collision(scene, 0.4, tank1, w);
            create_physics_collision(scene, 0.4, tank2, w);
        }
    }

    // add bodies to the scene
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


// checks to see if tanks are still in the scene
bool game_over(Scene *scene) {
    bool tank1_alive = false;
    bool tank2_alive = false;
    for (int i = 0; i < scene_bodies(scene); i++) {
        if (get_nth_bodytype(scene, i) == ONE) {
            tank1_alive = true;
        }
        if (get_nth_bodytype(scene, i) == TWO) {
            tank2_alive = true;
        }
    }

    if (tank1_alive == false) {
        Mix_PlayChannel( -1, boom, 0 );
        printf("Red Tank wins!\n");
    }

    if (tank2_alive == false) {
        Mix_PlayChannel( -1, boom, 0 );
        printf("Blue Tank wins!\n");
    }

    return !(tank1_alive && tank2_alive);

}

bool check_explosion(Scene *scene) {
    for (int i = 0; i < scene_bodies(scene); i++) {
        if (get_nth_bodytype(scene, i) == EXPLOSION) {
            return true;
        }
    }
    return false;
}

// Start the game and return all scene components
Scene *create_game() {
    Scene *scene = scene_init();
    // system("afplay /Users/matthewkim/Desktop/CS03/tankwars/sounds/background.wav");
    sdl_init(min, max);
    sdl_on_key(on_key, scene);
    draw_background(scene);
    draw_boundaries(scene);
    draw_walls(scene);
    draw_tanks(scene);
    return scene;
}

// Restart the game and return all scene components
void restart_game(Scene *scene) {
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        scene_remove_body(scene, i);
    }
    sdl_render_scene(scene);
    draw_background(scene);
    draw_boundaries(scene);
    draw_walls(scene);
    draw_tanks(scene);
}

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;

    // Stroing start time
    clock_t start_time = clock();

    // looping till required time is not acheived
    while (clock() < start_time + milli_seconds);
}

int main(int argc, char *argv[]) {
    Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 );
    music = Mix_LoadMUS("sounds/background.wav");
    shoot = Mix_LoadWAV("sounds/quick2.wav");
    boom = Mix_LoadWAV("sounds/Explosion+3.wav");
    Mix_PlayMusic( music, -1 );
    double dt;
    Scene *scene = create_game();
    //system("eog ../images/explosions.png");
    while (!sdl_is_done()) {
        dt = time_since_last_tick();
        update_turret(scene);
        scene_tick(scene, dt);
        sdl_render_scene(scene);
        if (game_over(scene)) {
            delay(1000);
            restart_game(scene);
        }
    }

    Mix_FreeMusic(music);
    Mix_FreeChunk(shoot);
    Mix_FreeChunk(boom);
    return 0;
}
