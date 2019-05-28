#ifndef __COLOR_H__
#define __COLOR_H__
#include <stddef.h>

/**
 * A color to display on the screen.
 * The color is represented by its red, green, and blue components.
 * Each component must be between 0 (black) and 1 (white).
 */
typedef struct {
    float r;
    float g;
    float b;
} RGBColor;

RGBColor random_color();

RGBColor get_color(size_t i);

#endif // #ifndef __COLOR_H__
