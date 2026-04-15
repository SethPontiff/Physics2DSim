#include "PhysicsWorld.h"
#include "Vector2.h"
#include <algorithm>

PhysicsWorld::PhysicsWorld() {}

PhysicsWorld::~PhysicsWorld() {
    for (auto* body : bodies) {
        delete body;
    }
}

void PhysicsWorld::addBody(RigidBody* body) {
    bodies.push_back(body);
}

void PhysicsWorld::step(float dt) {
    for (auto* body : bodies) {
        body->integrate(dt);
    }

    for (size_t i = 0; i < bodies.size(); i++) {
        for (size_t j = i + 1; j < bodies.size(); j++) {
            sf::Vector2f normal;
            float penetration;

            if (checkCollision(bodies[i], bodies[j], normal, penetration)) {
                resolveCollision(bodies[i], bodies[j], normal, penetration);
            }
        }
    }

    for (auto* body : bodies) {
        body->clearForces();
    }
}

bool PhysicsWorld::checkCollision(RigidBody* a, RigidBody* b,
    sf::Vector2f& normal, float& penetration) {
    sf::Vector2f diff = b->position - a->position;
    float distance = Vec2::length(diff);
    float radiusSum = a->radius + b->radius;

    if (distance < radiusSum) {
        penetration = radiusSum - distance;

        if (distance > EPSILON) {
            normal = Vec2::normalized(diff);
        }
        else {
            normal = sf::Vector2f(1.f, 0.f); 
        }

        return true;
    }

    return false;
}

void PhysicsWorld::resolveCollision(RigidBody* a, RigidBody* b,
    const sf::Vector2f& normal, float penetration) {
    float totalInverseMass = a->getInverseMass() + b->getInverseMass();
    if (totalInverseMass == 0.0f) return; 

    float percent = POSITION_CORRECTION_PERCENT;
    float slop = POSITION_CORRECTION_SLOP;

    sf::Vector2f correction = normal * (std::max(penetration - slop, 0.0f) / totalInverseMass) * percent;

    a->position -= correction * a->getInverseMass();
    b->position += correction * b->getInverseMass();

    sf::Vector2f relativeVel = b->velocity - a->velocity;
    float velAlongNormal = Vec2::dot(relativeVel, normal);

    if (velAlongNormal > 0) return;

    float e = std::min(a->restitution, b->restitution);

    float j = -(1.0f + e) * velAlongNormal;
    j /= totalInverseMass;

    sf::Vector2f impulse = normal * j;
    a->velocity -= impulse * a->getInverseMass();
    b->velocity += impulse * b->getInverseMass();
}