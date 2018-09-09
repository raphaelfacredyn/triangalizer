//
// Created by raphael on 9/5/18.
//

#include "PointLine.h"

namespace rf {
    PointLine::PointLine(Vertex one, Vertex two) {
        this->one = one;
        this->two = two;
    }

    float PointLine::slope() const {
        return (one.y - two.y) / (one.x - two.x);
    }

    Vertex PointLine::midpoint() const {
        return Vertex((one.x + two.x) / 2, (one.y + two.y) / 2);
    }

    SlopeLine PointLine::perpendicularBisector() const {
        const float m = -1 / this->slope();
        const Vertex midpoint = this->midpoint();
        const float b = midpoint.y - m * midpoint.x;
        return SlopeLine(m, b);
    }
}