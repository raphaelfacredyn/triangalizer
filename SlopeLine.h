//
// Created by raphael on 9/5/18.
//

#pragma once

#include "Vertex.h"

namespace rf {
    class SlopeLine {
        float m;
        float b;

    public:
        SlopeLine(float m, float b);

        Vertex intersect(SlopeLine other) const;
    };
}