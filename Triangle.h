//
// Created by raphael on 9/5/18.
//

#pragma once

#include <SFML/Graphics.hpp>
#include "Vertex.h"
#include "vector"

namespace rf {
    class Triangle : public sf::ConvexShape {

    public:
        Vertex circumcenter;

        float circumradius;

        bool colorSet = false;

        std::vector<Vertex *> pointsInside;

        Triangle(Vertex one, Vertex two, Vertex three, bool calcPointsInside = true);

        void updateValues(bool calcPointsInside);

        void calcPointsInside();

        bool operator==(const Triangle& other) const;

        float area();
    };
}