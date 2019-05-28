#include <stdlib.h>
#include <math.h>
#include "collision.h"

#define UNDEFINED_VEC (Vector) {0, 0}
#define DEFAULT_OVERLAP 0



// Returns a normalized axis as a vector given an edge of a shape (two vertices)
Vector get_axis(Vector p1, Vector p2) {
    // gets the norm of the edge between vertices p1 and p2
    Vector norm_edge = vec_norm(vec_subtract(p2, p1));
    return vec_normalize(norm_edge);
}


// Returns projection of a given shape to a given axis
Projection get_projection(List *shape, Vector axis) {
    double min = vec_dot(*(Vector *) list_get(shape, 0), axis);
    double max = min;

    for (size_t i = 1; i < list_size(shape); i++) {
        double p = vec_dot(*(Vector *) list_get(shape, i), axis);

        if (p < min) {
            min = p;
        } else if (p > max) {
            max = p;
        }
    }
    return (Projection) {.min = min, .max = max};
}


/**
 * Determines if projection of two shapes onto all the axis of shape_primary
 * overlaps and returns the CollisioInfo with axis onto which the shapes'
 * projections have least overlap
 */
CollisionInfo projections_overlap(List *shape_primary,
    List *shape_secondary, double min_val, Vector prev_axis)
{
    double min_overlap = min_val;
    Vector col_axis = prev_axis;
    size_t shape_size = list_size(shape_primary);
    for (size_t i = 0; i < shape_size; i++) {
        Vector v1 = *(Vector *) list_get(shape_primary, i);
        Vector v2 = *(Vector *) list_get(shape_primary, (i + 1) % shape_size);
        Vector axis = get_axis(v1, v2);
        Projection p1 = get_projection(shape_primary, axis);
        Projection p2 = get_projection(shape_secondary, axis);

        if (!overlaps(p1, p2)) {
            return (CollisionInfo) {.collided = false, .min_overlap = 0,
                .axis = col_axis};
        }

        else {
            // amount in which the projections overlap by
            double diff = getOverlap(p1, p2);
            if (diff < min_overlap) {
                min_overlap = diff;
                col_axis = axis;
            }
        }
    }

    return (CollisionInfo) {.collided = true, .min_overlap = min_overlap,
        .axis = col_axis};
}


CollisionInfo find_collision(List *shape1, List *shape2) {
    // really large min_overlap value
    CollisionInfo c1 = projections_overlap(shape1, shape2,
                                             1000, UNDEFINED_VEC);

    if (c1.collided) {
        CollisionInfo c2 = projections_overlap(shape2, shape1,
                                            c1.min_overlap, c1.axis);
        if (c2.collided) {
            return (CollisionInfo) {.collided = true, .axis = c2.axis, DEFAULT_OVERLAP};
        }
    }

    return (CollisionInfo) {.collided = false, .axis = UNDEFINED_VEC, DEFAULT_OVERLAP};

}
