#include <stdlib.h>
#include <math.h>
#include "color.h"

float get_rand() {
    return rand() / (double) RAND_MAX;
}

// Returns a random RGB color
RGBColor random_color() {
    return (RGBColor) {.r = get_rand(), .g = get_rand(), .b = get_rand()};
}

// Returns colors of a rainbow
RGBColor get_color(size_t i) {
    RGBColor color;
    switch (i) {
        case 0:
            color = (RGBColor) {.r = 1, .g = 0, .b = 0};
            break;
        case 1:
            color = (RGBColor) {.r = 1, .g = 0.4, .b = 0};
            break;
        case 2:
            color = (RGBColor) {.r = 1, .g = 0.8, .b = 0};
            break;
        case 3:
            color = (RGBColor) {.r = 0.8, .g = 1, .b = 0};
            break;
        case 4:
            color = (RGBColor) {.r = 0.4, .g = 1, .b = 0};
            break;
        case 5:
            color = (RGBColor) {.r = 0, .g = 1, .b = 0};
            break;
        case 6:
            color = (RGBColor) {.r = 0, .g = 1, .b = 0.4};
            break;
        case 7:
            color = (RGBColor) {.r = 0, .g = 1, .b = 0.8};
            break;
        case 8:
            color = (RGBColor) {.r = 0, .g = 0.8, .b = 1};
            break;
        case 9:
            color = (RGBColor) {.r = 0, .g = 0.4, .b = 1};
            break;
        default:
            color = (RGBColor) {.r = 0, .g = 0, .b = 1};
    }
    return color;
}
