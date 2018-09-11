#include <iostream>
#include "Triangle.h"
#include <utility>
#include <vector>
#include <SFML/Graphics.hpp>
#include <thread>
#include <mutex>
#include "addons.h"
#include <cstdlib>
#include <cmath>

using namespace rf;

// <triangles, incomplete points>
void triangalize(std::vector<Triangle *> &triangles, std::mutex &trianglesMutex, std::vector<Vertex *> &innerPoints,
                 std::vector<Vertex *> &points, std::mutex &pointsMutex, std::vector<bool> &threadsDone,
                 std::mutex &threadsDoneMutex, long threadIndex) {
    for (auto p0 : innerPoints) {
        if (!(p0->completed())) {
            for (auto p1 : innerPoints) {
                if (p1 != p0 && !p1->completed()) {
                    for (auto p2 : innerPoints) {
                        if (p2 != p0 && p2 != p1 && !p2->completed()) {
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
    }

    threadsDoneMutex.lock();
    threadsDone[threadIndex] = true;
    threadsDoneMutex.unlock();
}

void findEdges(std::vector<Vertex *> &points, sf::Image &image) {
    int kernel[3][3] = {
            {-1, -1, -1},
            {-1, 8,  -1},
            {-1, -1, -1}
    };
    float edges[image.getSize().y][image.getSize().x];
    for (int row = 1; row < image.getSize().y - 1; row++) {
        for (int col = 1; col < image.getSize().x - 1; col++) {
            float accumulation = 0;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    sf::Color c = image.getPixel(static_cast<unsigned int>(col + i),
                                                 static_cast<unsigned int>(row + j));
                    float k = (c.r + c.g + c.b) / 3.0f;
                    accumulation += k * kernel[1 + i][1 + j];
                }
            }
            edges[row][col] += accumulation;
        }
    }

    for (int row = 0; row < sizeof(edges) / sizeof(*edges); ++row) {
        for (int col = 0; col < sizeof(edges[row]) / sizeof(*(edges[row])); ++col) {
            if (abs(edges[row][col]) > 40 && rand() % 1000 == 1) // Play around with these numbers
                points.push_back(new Vertex(col, row));
        }
    }

    std::cout << points.size() << std::endl;
}

int main() {
    std::vector<Vertex *> points;
    std::mutex pointsMutex;

    sf::Image image;
    image.loadFromFile("/home/raphael/Pictures/Raph.jpg");

    unsigned int width = image.getSize().x, height = image.getSize().y;
    int sectorWidth = width / 2, sectorHeight = height / 2;

    srand(static_cast<unsigned int>(time(nullptr)));

    findEdges(points, image);

    for (int i = 0; i < width; i++) {
        if (i % 50 == 0) {
            points.push_back(new Vertex(i, 0));
            points.push_back(new Vertex(i, height - 1));
        }
    }
    for (int i = 0; i < height; i++) {
        if (i % 50 == 0) {
            points.push_back(new Vertex(0, i));
            points.push_back(new Vertex(width - 1, i));
        }
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
                sumR /= t->pointsInside.size();
                sumG /= t->pointsInside.size();
                sumB /= t->pointsInside.size();
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

                if (!(p->completed())) {
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
                if (!(point->completed()))
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