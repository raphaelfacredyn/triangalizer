//
// Created by raphael on 9/8/18.
//

#include "Vertex.h"
#include <math.h>

namespace rf {
    Vertex::Vertex() = default;

    Vertex::Vertex(sf::Vector2f v) : sf::Vector2f(v.x, v.y) {}

    Vertex::Vertex(float x, float y) : Vector2(x, y) {}

    float Vertex::dist(const Vertex other) const {
        return sqrtf(powf(other.x - x, 2) + powf(other.y - y, 2));
    }

    float Vertex::magnitude() const {
        return sqrtf(powf(x, 2) + powf(y, 2));
    }

    bool Vertex::within(const Vertex &min, const Vertex &max) const {
        return x >= min.x && y >= min.y && x <= max.x && y <= max.y;
    }

    bool Vertex::completed() {
        return angle >= M_PI * 2 - 0.1;
    }
}
