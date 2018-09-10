//
// Created by raphael on 9/5/18.
//

#include "Triangle.h"
#include "SlopeLine.h"
#include "PointLine.h"
#include "math.h"

namespace rf {
    Triangle::Triangle(Vertex one, Vertex two, Vertex three, bool calcPointsInside) {
        setPointCount(3);
        setPoint(0, one);
        setPoint(1, two);
        setPoint(2, three);

        updateValues(calcPointsInside);
    }

    void Triangle::updateValues(bool calcPointsInside) {
        Vertex ptOne = getPoint(0);
        Vertex ptTwo = getPoint(1);
        Vertex ptThree = getPoint(2);
        if (ptOne.x == ptTwo.x || ptTwo.x == ptTwo.x) {
            ptOne = getPoint(2);
            ptTwo = getPoint(0);
            ptThree = getPoint(1);
        }
        PointLine one = PointLine(ptOne, ptTwo);
        PointLine two = PointLine(ptTwo, ptThree);

        circumcenter = one.perpendicularBisector().intersect(two.perpendicularBisector());
        circumradius = circumcenter.dist(ptOne);

        if(calcPointsInside)
            this->calcPointsInside();
    }

    void Triangle::calcPointsInside() {
        Vertex ptOne = getPoint(0);
        Vertex ptTwo = getPoint(1);
        Vertex ptThree = getPoint(2);

        Vertex minBound = Vertex(fmin(fmin(ptOne.x, ptTwo.x), ptThree.x), fmin(fmin(ptOne.y, ptTwo.y), ptThree.y));
        Vertex maxBound = Vertex(fmax(fmax(ptOne.x, ptTwo.x), ptThree.x), fmax(fmax(ptOne.y, ptTwo.y), ptThree.y));

        for (int x = static_cast<int>(minBound.x); x <= maxBound.x; x++) {
            for (int y = static_cast<int>(minBound.y); y <= maxBound.y; y++) {
                auto p = new Vertex(x, y);
                if (abs(area() - Triangle(*p, ptOne, ptTwo, false).area() - Triangle(*p, ptOne, ptThree, false).area() - Triangle(*p, ptTwo, ptThree, false).area()) < 0.1)
                    pointsInside.push_back(p);
                else
                    delete p;
            }
        }
    }

    bool Triangle::operator==(const Triangle &other) const {
        if (getPoint(0) == other.getPoint(0) && getPoint(1) == other.getPoint(1) && getPoint(2) == other.getPoint(2))
            return true;
        if (getPoint(0) == other.getPoint(0) && getPoint(1) == other.getPoint(2) && getPoint(2) == other.getPoint(1))
            return true;
        if (getPoint(0) == other.getPoint(1) && getPoint(1) == other.getPoint(2) && getPoint(2) == other.getPoint(0))
            return true;
        if (getPoint(0) == other.getPoint(1) && getPoint(1) == other.getPoint(0) && getPoint(2) == other.getPoint(2))
            return true;
        if (getPoint(0) == other.getPoint(2) && getPoint(1) == other.getPoint(1) && getPoint(2) == other.getPoint(0))
            return true;
        return getPoint(0) == other.getPoint(2) && getPoint(1) == other.getPoint(0) && getPoint(2) == other.getPoint(1);
    }

    float Triangle::area() {
        return abs((getPoint(0).x * (getPoint(1).y - getPoint(2).y) + getPoint(1).x * (getPoint(2).y - getPoint(0).y) +
                    getPoint(2).x * (getPoint(0).y - getPoint(1).y)) / 2.0f);
    }
}