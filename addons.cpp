//
// Created by raphael on 9/7/18.
//

#include "addons.h"
#include <math.h>

namespace rf {
    float angle(const Vertex a, const Vertex b, const Vertex c) {
        const Vertex shiftedA = a - b;
        const Vertex shiftedC = c - b;
        return acosf(
                (shiftedA.x * shiftedC.x + shiftedA.y * shiftedC.y) / (shiftedA.magnitude() * shiftedC.magnitude()));
    }
}