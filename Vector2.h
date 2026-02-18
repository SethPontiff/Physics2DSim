#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>

// Just some helper functions.
namespace Vec2 {
    inline float dot(const sf::Vector2f& a, const sf::Vector2f& b) {
        return a.x * b.x + a.y * b.y;
    }

    inline float length(const sf::Vector2f& v) {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

    inline float lengthSquared(const sf::Vector2f& v) {
        return v.x * v.x + v.y * v.y;
    }

    inline sf::Vector2f normalized(const sf::Vector2f& v) {
        float len = length(v);
        if (len > 0.0001f) {
            return sf::Vector2f(v.x / len, v.y / len);
        }
        return sf::Vector2f(0.f, 0.f);
    }

    inline float distance(const sf::Vector2f& a, const sf::Vector2f& b) {
        return length(b - a);
    }
}