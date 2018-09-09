//
// Created by raphael on 9/5/18.
//

#include "SlopeLine.h"

using namespace std;

namespace rf {
    SlopeLine::SlopeLine(float m, float b) {
        this->m = m;
        this->b = b;
    }

    Vertex SlopeLine::intersect(SlopeLine other) const {
        float x = (b - other.b) / (other.m - m);
        return Vertex(x, m * x + b);
    }
}