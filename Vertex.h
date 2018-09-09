//
// Created by raphael on 9/8/18.
//

#pragma once

#include <SFML/Graphics.hpp>

namespace rf {
    class Vertex : public sf::Vector2f {
    public:
        float angle = 0;

        Vertex(sf::Vector2f v);

        Vertex(float x, float y);

        Vertex();

        float dist(Vertex other) const;

        float magnitude() const;

        bool within(const Vertex &min, const Vertex &max) const;

        bool completed();
    };
}
