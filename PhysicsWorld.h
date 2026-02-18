#pragma once
#include "RigidBody.h"
#include <vector>

class PhysicsWorld {
private:
    std::vector<RigidBody*> bodies;

    bool checkCollision(RigidBody* a, RigidBody* b,
        sf::Vector2f& normal, float& penetration);

    void resolveCollision(RigidBody* a, RigidBody* b,
        const sf::Vector2f& normal, float penetration);

public:
    PhysicsWorld();
    ~PhysicsWorld();

    void addBody(RigidBody* body);
    const std::vector<RigidBody*>& getBodies() const { return bodies; }

    void step(float dt);
};