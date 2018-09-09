//
// Created by raphael on 9/5/18.
//

#include "Triangle.h"
#include "SlopeLine.h"
#include "PointLine.h"

namespace rf {
    Triangle::Triangle(Vertex one, Vertex two, Vertex three) {
        setPointCount(3);
        setPoint(0, one);
        setPoint(1, two);
        setPoint(2, three);

        updateValues();
    }

    void Triangle::updateValues() {
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
    }

    bool Triangle::operator==(const Triangle &other) const{
        if(getPoint(0) == other.getPoint(0) && getPoint(1) == other.getPoint(1) && getPoint(2) == other.getPoint(2))
            return true;
        if(getPoint(0) == other.getPoint(0) && getPoint(1) == other.getPoint(2) && getPoint(2) == other.getPoint(1))
            return true;
        if(getPoint(0) == other.getPoint(1) && getPoint(1) == other.getPoint(2) && getPoint(2) == other.getPoint(0))
            return true;
        if(getPoint(0) == other.getPoint(1) && getPoint(1) == other.getPoint(0) && getPoint(2) == other.getPoint(2))
            return true;
        if(getPoint(0) == other.getPoint(2) && getPoint(1) == other.getPoint(1) && getPoint(2) == other.getPoint(0))
            return true;
        return getPoint(0) == other.getPoint(2) && getPoint(1) == other.getPoint(0) && getPoint(2) == other.getPoint(1);
    }
}