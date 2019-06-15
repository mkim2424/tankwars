#include "scene.h"
#include "sdl_wrapper.h"

#define INIT_SIZE 10

/**
 * force_creator_info struct to hold information about a force creator.
 *
 * @param forcer ForceCreator function to by called from scene_tick().
 * @param aux    auxilary value to pass to forcer.
 * @param freer  function to free aux.
 * @param bodies a list of bodies that this is applied to.
 */
typedef struct {
    ForceCreator forcer;
    void *aux;
    FreeFunc freer;
    List *bodies;
} force_creator_info;

/**
 * @param bodies         a list of bodies in the scene.
 * @param force_creators a list of force_creator_info's.
 */
struct scene {
    List *bodies;
    List *force_creators;
};

Scene *scene_init(void) {
    Scene *res = malloc(sizeof(Scene));
    assert(res != NULL);
    res->bodies = list_init(INIT_SIZE, (FreeFunc) body_free);
    res->force_creators = list_init(INIT_SIZE, (FreeFunc) free);
    return res;
}

void scene_free(Scene *scene) {
    assert(scene != NULL);
    list_free(scene->bodies);

    for (size_t i = 0; i < list_size(scene->force_creators); i++) {
        force_creator_info *tmp = list_get(scene->force_creators, i);
        if (tmp->freer != NULL && tmp->aux != NULL) {
            tmp->freer(tmp->aux);
        }

    }
    list_free(scene->force_creators);
    free(scene);
}

size_t scene_bodies(Scene *scene) {
    assert(scene != NULL);
    return list_size(scene->bodies);
}

Body *scene_get_body(Scene *scene, size_t index) {
    assert(scene != NULL && index < scene_bodies(scene));
    return list_get(scene->bodies, index);
}

void scene_add_body(Scene *scene, Body *body) {
    assert(scene != NULL && body != NULL);
    list_add(scene->bodies, body);
}

void scene_remove_body(Scene *scene, size_t index) {
    assert(scene != NULL);
    body_remove(list_get(scene->bodies, index));
    // body_free(list_remove(scene->bodies, index));
}

void scene_add_force_creator(Scene *scene, ForceCreator forcer,
    void *aux, FreeFunc freer) {
    scene_add_bodies_force_creator(scene, forcer, aux, list_init(10, free), freer);
}

/**
 * Adds a force creator to a scene,
 * to be invoked every time scene_tick() is called.
 * The auxiliary value is passed to the force creator each time it is called.
 * The force creator is registered with a list of bodies it applies to,
 * so it can be removed when any one of the bodies is removed.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param forcer a force creator function
 * @param aux an auxiliary value to pass to forcer when it is called
 * @param bodies the list of bodies affected by the force creator.
 *   The force creator will be removed if any of these bodies are removed.
 *   This list does not own the bodies, so its freer should be NULL.
 * @param freer if non-NULL, a function to call in order to free aux
 */
void scene_add_bodies_force_creator(
    Scene *scene, ForceCreator forcer, void *aux, List *bodies, FreeFunc freer
) {
    assert(scene != NULL);
    force_creator_info *res = malloc(sizeof(force_creator_info));
    assert(res != NULL);
    res->forcer = forcer;
    res->aux = aux;
    res->freer = freer;
    res->bodies = bodies;
    list_add(scene->force_creators, res);
}


BodyType get_bodytype(Scene *scene, size_t n) {
    Body_info *tmp = body_get_info(scene_get_body(scene, n));
    return (*tmp).b;
}

void make_delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;

    // Storing start time
    clock_t start_time = clock();

    // looping till required time is not acheived
    while (clock() < start_time + milli_seconds);
}

void scene_tick(Scene *scene, double dt) {
    assert(scene != NULL);
    size_t ind = 0;
    bool explosion = false;

    while (ind < list_size(scene->force_creators)) {
        force_creator_info *tmp = list_get(scene->force_creators, ind);
        // Flag whether this force creator should be removed
        bool remove_force_creator = false;
        // Apply force
        tmp->forcer(tmp->aux);

        for (size_t i = 0; i < list_size(tmp->bodies); i++) {
            Body *body_tmp = list_get(tmp->bodies, i);

            if (body_is_removed(body_tmp)) {
                remove_force_creator = true;
            }
        }

        ind++;
    }

    ind = 0;

    while (ind < list_size(scene->force_creators)) {
        force_creator_info *tmp = list_get(scene->force_creators, ind);
        // Flag whether this force creator should be removed
        bool remove_force_creator = false;

        for (size_t i = 0; i < list_size(tmp->bodies); i++) {
            Body *body_tmp = list_get(tmp->bodies, i);

            if (body_is_removed(body_tmp)) {
                remove_force_creator = true;
                break;
            }
        }

        if (remove_force_creator) {
            list_remove(scene->force_creators, ind);
        } else {
            ind++;
        }
    }

    ind = 0;

    for (int i = 0; i < scene_bodies(scene); i++) {
        if (get_bodytype(scene, i) == EXPLOSION) {
            explosion = true;
        }
    }

    while (ind < scene_bodies(scene)) {
        Body *body_tmp = scene_get_body(scene, ind);

        if (body_is_removed(body_tmp)) {
            Body_info *body_i = body_get_info(body_tmp);
            if (body_i->b == TWO && !explosion) {
                Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 );
                Mix_Chunk *boom = Mix_LoadWAV("sounds/Explosion+3.wav");
                Mix_FreeChunk(boom);
                Mix_PlayChannel( -1, boom, 0 );
                for (size_t i = 0; i < 5; i++) {
                    for (size_t j = 0; j <= i; j++) {
                        Body *b1 = star_shape(5+2*j, 200-((5-i)*10)-25*j,
                            100, (RGBColor) {.r = 1, .g = 1-j*0.2, .b = 0}, 
                            body_get_centroid(body_tmp), EXPLOSION);
                        scene_add_body(scene, b1);
                    }
                    make_delay(120);
                    sdl_render_scene(scene);
                }
            }
            else if (body_i->b == ONE && !explosion) {
                Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 );
                Mix_Chunk *boom = Mix_LoadWAV("sounds/Explosion+3.wav");
                Mix_FreeChunk(boom);
                Mix_PlayChannel( -1, boom, 0 );
                for (size_t i = 0; i < 5; i++) {
                    for (size_t j = 0; j <= i; j++) {
                        Body *b1 = star_shape(5+2*j, 200-((5-i)*10)-25*j,
                            100, (RGBColor) {.r = 1, .g = 1-j*0.2, .b = 0}, 
                            body_get_centroid(body_tmp), EXPLOSION);
                        scene_add_body(scene, b1);
                    }
                    make_delay(120);
                    sdl_render_scene(scene);
                }
            }
            list_remove(scene->bodies, ind);
            body_free(body_tmp);
        } else {
            body_tick(body_tmp, dt);
            ind++;
        }
    }
}
