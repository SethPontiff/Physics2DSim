#pragma once
#include <SFML/System/Vector2.hpp>

enum class ShapeType {
    Circle,
    AABB,
    Plane
};

class Shape {
public:
    virtual ~Shape() = default;
    virtual ShapeType getType() const = 0;
};

class CircleShape : public Shape {
public:
    float radius;
    explicit CircleShape(float r) : radius(r) {}
    ShapeType getType() const override { return ShapeType::Circle; }
};

class AABBShape : public Shape {
public:
    float width;
    float height;
    AABBShape(float w, float h) : width(w), height(h) {}
    ShapeType getType() const override { return ShapeType::AABB; }
};

class PlaneShape : public Shape {
public:
    sf::Vector2f normal;
    sf::Vector2f point;
    PlaneShape(sf::Vector2f normal, sf::Vector2f point)
        : normal(normal), point(point) {}
    ShapeType getType() const override { return ShapeType::Plane; }
};