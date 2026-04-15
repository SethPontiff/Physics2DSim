#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

class RigidBody {
public:
    static constexpr float MAX_VELOCITY = 800.0f;
    static constexpr float VELOCITY_STOP_THRESHOLD_SQUARED = 0.1f;

    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f force; 

    float mass;
    float radius;
    float restitution; 
    float friction;  

    bool isStatic;
    sf::Color color;

    RigidBody(sf::Vector2f pos, float mass, float radius, bool isStatic = false);

    void applyForce(const sf::Vector2f& f);
    void applyImpulse(const sf::Vector2f& impulse);
    void integrate(float dt);
    void clearForces();

    float getInverseMass() const {
        return isStatic ? 0.0f : 1.0f / mass;
    }
};