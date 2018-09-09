//
// Created by raphael on 9/5/18.
//

#pragma once

#include <SFML/Graphics.hpp>
#include "Vertex.h"

namespace rf {
    class Triangle : public sf::ConvexShape {

    public:
        Vertex circumcenter;

        float circumradius;

        sf::Color color;

        Triangle(Vertex one, Vertex two, Vertex three);

        void updateValues();

        bool operator==(const Triangle& other) const;
    };
}