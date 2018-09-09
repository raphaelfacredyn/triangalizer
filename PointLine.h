//
// Created by raphael on 9/5/18.
//

#pragma once

#include "SlopeLine.h"
#include "Vertex.h"

namespace rf {
    class PointLine {
        Vertex one;
        Vertex two;

    public:
        PointLine(Vertex one, Vertex two);

        float slope() const;

        Vertex midpoint() const;

        SlopeLine perpendicularBisector() const;
    };
}