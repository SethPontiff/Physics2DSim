#include "RigidBody.h"
#include "Vector2.h" 

RigidBody::RigidBody(sf::Vector2f pos, float mass, float radius, bool isStatic)
    : position(pos),
    velocity(0.f, 0.f),
    force(0.f, 0.f),
    mass(mass),
    radius(radius),
    restitution(0.85f),  
    friction(0.98f),   
    isStatic(isStatic),
    color(sf::Color::White)
{}

void RigidBody::applyForce(const sf::Vector2f& f) {
    if (!isStatic) {
        force += f;
    }
}

void RigidBody::applyImpulse(const sf::Vector2f& impulse) {
    if (!isStatic) {
        velocity += impulse * getInverseMass();
    }
}

void RigidBody::integrate(float dt) {
    if (isStatic) return;

    sf::Vector2f acceleration = force * getInverseMass();

    velocity += acceleration * dt;

    velocity *= friction;

    float maxSpeed = 1500.0f; // Can't make this too high, otherwise it'll shoot into the stratosphere.
    float speed = Vec2::length(velocity);
    if (speed > maxSpeed) {
        velocity = Vec2::normalized(velocity) * maxSpeed;
    }

    if (Vec2::lengthSquared(velocity) < 0.1f) {
        velocity = sf::Vector2f(0.f, 0.f);
    }

    position += velocity * dt;
}

void RigidBody::clearForces() {
    force = sf::Vector2f(0.f, 0.f);
}