#include <iostream>
#include "Triangle.h"
#include <utility>
#include <vector>
#include <SFML/Graphics.hpp>
#include <thread>
#include <mutex>
#include "addons.h"
#include <cstdlib>
#include <math.h>

using namespace rf;

// <triangles, incomplete points>
void triangalize(std::vector<Triangle *> &triangles, std::mutex &trianglesMutex, std::vector<Vertex *> &innerPoints,
                 std::vector<Vertex *> &points, std::mutex &pointsMutex, std::vector<bool> &threadsDone,
                 std::mutex &threadsDoneMutex, long threadIndex) {
    for (auto p0 : innerPoints) {
        for (auto p1 : innerPoints) {
            if (p1 != p0) {
                for (auto p2 : innerPoints) {
                    if (p2 != p0 && p2 != p1) {
                        auto t = new Triangle(*p0, *p1, *p2, false);
                        bool good = true;

                        for (auto other : triangles) {
                            good &= !(*other == *t);
                        }

                        if (good) {
                            for (auto other : points) {
                                good &= t->circumradius <= t->circumcenter.dist(*other);
                            }
                        }
                        if (good) {
                            t->setFillColor(sf::Color(255, 0, 0));

                            t->calcPointsInside();

                            trianglesMutex.lock();
                            triangles.push_back(t);
                            trianglesMutex.unlock();

                            float angle0 = angle(t->getPoint(1), t->getPoint(0), t->getPoint(2));
                            float angle1 = angle(t->getPoint(0), t->getPoint(1), t->getPoint(2));
                            float angle2 = angle(t->getPoint(1), t->getPoint(2), t->getPoint(0));

                            pointsMutex.lock();
                            p0->angle += angle0;
                            p1->angle += angle1;
                            p2->angle += angle2;
                            pointsMutex.unlock();
                        } else {
                            delete t;
                        }
                    }
                }
            }
        }
    }

    threadsDoneMutex.lock();
    threadsDone[threadIndex] = true;
    threadsDoneMutex.unlock();
}

int main() {
    std::vector<Vertex *> points;
    std::mutex pointsMutex;

    sf::Image image;
    image.loadFromFile("/home/raphael/Pictures/Raph.jpg");

    unsigned int width = image.getSize().x, height = image.getSize().y;
    int sectorWidth = width / 4, sectorHeight = height / 4;

    srand(static_cast<unsigned int>(time(nullptr)));
    for (int i = 0; i < 400; i++) {
        points.push_back(new Vertex(rand() % width, rand() % height));
    }

    std::vector<Triangle *> triangles;
    std::mutex trianglesMutex;

    std::vector<bool> threadsDone;
    std::mutex threadsDoneMutex;
    for (int w = 0; w < width / sectorWidth; w++) {
        for (int h = 0; h < height / sectorHeight; h++) {
            threadsDone.push_back(false);

            auto innerPoints = new std::vector<Vertex *>;
            pointsMutex.lock();
            for (auto point : points)
                if (point->within(Vertex(w * sectorWidth, h * sectorHeight),
                                  Vertex((w + 1) * sectorWidth, (h + 1) * sectorHeight)))
                    innerPoints->push_back(point);
            pointsMutex.unlock();

            new std::thread(triangalize, std::ref(triangles), std::ref(trianglesMutex), std::ref(*innerPoints),
                            std::ref(points), std::ref(pointsMutex), std::ref(threadsDone), std::ref(threadsDoneMutex),
                            threadsDone.size() - 1);
        }
    }

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(width, height), "Triangalizer", sf::Style::Titlebar | sf::Style::Close,
                            settings);
    window.setFramerateLimit(30);

    bool didPostDone = false;

    bool drawCircles = false;
    bool drawOutlines = false;
    bool drawPoints = false;

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape))
                window.close();
            if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == 2)
                    drawCircles = !drawCircles;
                if (event.key.code == 14)
                    drawOutlines = !drawOutlines;
                if (event.key.code == 15)
                    drawPoints = !drawPoints;
            }
        }

        window.clear(sf::Color(255, 255, 255));

        pointsMutex.lock();
        trianglesMutex.lock();

        for (auto t : triangles) {
            t->setOutlineThickness(drawOutlines);
            t->setOutlineColor(sf::Color(0, 0, 255, static_cast<sf::Uint8>(drawOutlines * 255)));
        }

        for (auto t : triangles) {
            if (!(t->colorSet)) {
                long sumR = 0;
                long sumG = 0;
                long sumB = 0;
                for (auto p: t->pointsInside) {
                    sf::Color c = image.getPixel(static_cast<unsigned int>(p->x), static_cast<unsigned int>(p->y));
                    sumR += static_cast<int>(pow(c.r, 2));
                    sumG += static_cast<int>(pow(c.g, 2));
                    sumB += static_cast<int>(pow(c.b, 2));
                }
                t->setFillColor(sf::Color(static_cast<sf::Uint8>(sqrt(sumR)), static_cast<sf::Uint8>(sqrt(sumG)),
                                          static_cast<sf::Uint8>(sqrt(sumB))));
                t->colorSet = true;
            }
            window.draw(*t);
        }

        if (drawPoints)
            for (auto p : points) {
                sf::CircleShape shape(3);
                shape.setPosition(*p - Vertex(3, 3));
                shape.setFillColor(sf::Color(0, 255, 0));

                if (p->angle < 2 * M_PI - 0.1) {
                    shape.setFillColor(sf::Color(100, 100, 100));
                }

                window.draw(shape);
            }

        if (drawCircles)
            for (auto t : triangles) {
                sf::CircleShape shape(t->circumradius);
                shape.setPosition(t->circumcenter - Vertex(t->circumradius, t->circumradius));
                shape.setFillColor(sf::Color(0, 0, 0, 0));
                shape.setOutlineThickness(1);
                shape.setOutlineColor(sf::Color(0, 0, 0));
                window.draw(shape);
            }

        bool done = true;

        for (auto b : threadsDone) {
            done &= b;
        }

        if (done && !didPostDone) {
            auto unusedPoints = new std::vector<Vertex *>;
            for (auto point : points)
                if (point->angle < M_PI * 2 - 0.1)
                    unusedPoints->push_back(point);

            new std::thread(triangalize, std::ref(triangles), std::ref(trianglesMutex), std::ref(*unusedPoints),
                            std::ref(points), std::ref(pointsMutex), std::ref(threadsDone), std::ref(threadsDoneMutex),
                            threadsDone.size() - 1);
            didPostDone = true;
        }

        pointsMutex.unlock();
        trianglesMutex.unlock();

        window.display();
    }

    return 0;
}