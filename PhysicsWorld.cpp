#include "PhysicsWorld.h"
#include "Vector2.h"
#include <algorithm>
#include <cmath>

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
            float penetration = 0.f;

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
    ShapeType typeA = a->shape->getType();
    ShapeType typeB = b->shape->getType();

    // Circle - Circle
    if (typeA == ShapeType::Circle && typeB == ShapeType::Circle)
        return checkCircleCircle(a, b, normal, penetration);

    // Circle - AABB
    if (typeA == ShapeType::Circle && typeB == ShapeType::AABB)
        return checkCircleAABB(a, b, normal, penetration);
    if (typeA == ShapeType::AABB && typeB == ShapeType::Circle) {
        bool result = checkCircleAABB(b, a, normal, penetration);
        normal = -normal;
        return result;
    }

    // Circle - Plane
    if (typeA == ShapeType::Circle && typeB == ShapeType::Plane)
        return checkCirclePlane(a, b, normal, penetration);
    if (typeA == ShapeType::Plane && typeB == ShapeType::Circle) {
        bool result = checkCirclePlane(b, a, normal, penetration);
        normal = -normal;
        return result;
    }

    // AABB - AABB
    if (typeA == ShapeType::AABB && typeB == ShapeType::AABB)
        return checkAABBAABB(a, b, normal, penetration);

    // AABB - Plane
    if (typeA == ShapeType::AABB && typeB == ShapeType::Plane)
        return checkAABBPlane(a, b, normal, penetration);
    if (typeA == ShapeType::Plane && typeB == ShapeType::AABB) {
        bool result = checkAABBPlane(b, a, normal, penetration);
        normal = -normal;
        return result;
    }

    return false;
}

bool PhysicsWorld::checkCircleCircle(RigidBody* a, RigidBody* b,
    sf::Vector2f& normal, float& penetration) {
    CircleShape* circleA = a->getShapeAs<CircleShape>();
    CircleShape* circleB = b->getShapeAs<CircleShape>();

    sf::Vector2f diff = b->position - a->position;
    float distance = Vec2::length(diff);
    float radiusSum = circleA->radius + circleB->radius;

    if (distance < radiusSum) {
        penetration = radiusSum - distance;
        normal = (distance > EPSILON) ? Vec2::normalized(diff) : sf::Vector2f(1.f, 0.f);
        return true;
    }

    return false;
}

bool PhysicsWorld::checkCircleAABB(RigidBody* circle, RigidBody* aabb,
    sf::Vector2f& normal, float& penetration) {
    CircleShape* circleShape = circle->getShapeAs<CircleShape>();
    AABBShape* aabbShape = aabb->getShapeAs<AABBShape>();

    float halfW = aabbShape->width / 2.f;
    float halfH = aabbShape->height / 2.f;

    sf::Vector2f closest;
    closest.x = std::max(aabb->position.x - halfW,
        std::min(circle->position.x, aabb->position.x + halfW));
    closest.y = std::max(aabb->position.y - halfH,
        std::min(circle->position.y, aabb->position.y + halfH));

    sf::Vector2f diff = circle->position - closest;
    float distSq = Vec2::lengthSquared(diff);
    float radiusSq = circleShape->radius * circleShape->radius;

    if (distSq < radiusSq) {
        float distance = std::sqrt(distSq);

        if (distance > EPSILON) {
            normal = Vec2::normalized(diff);
            penetration = circleShape->radius - distance;
        }
        else {
            float overlapX = halfW - std::abs(circle->position.x - aabb->position.x);
            float overlapY = halfH - std::abs(circle->position.y - aabb->position.y);

            if (overlapX < overlapY) {
                normal = sf::Vector2f(circle->position.x > aabb->position.x ? 1.f : -1.f, 0.f);
                penetration = overlapX;
            }
            else {
                normal = sf::Vector2f(0.f, circle->position.y > aabb->position.y ? 1.f : -1.f);
                penetration = overlapY;
            }
        }
        return true;
    }

    return false;
}

bool PhysicsWorld::checkCirclePlane(RigidBody* circle, RigidBody* plane,
    sf::Vector2f& normal, float& penetration) {
    CircleShape* circleShape = circle->getShapeAs<CircleShape>();
    PlaneShape* planeShape = plane->getShapeAs<PlaneShape>();

    sf::Vector2f toCircle = circle->position - planeShape->point;
    float signedDist = Vec2::dot(toCircle, planeShape->normal);

    if (signedDist > 0.f && signedDist < circleShape->radius) {
        normal = -planeShape->normal;
        penetration = circleShape->radius - signedDist;
        return true;
    }

    return false;
}

bool PhysicsWorld::checkAABBAABB(RigidBody* a, RigidBody* b,
    sf::Vector2f& normal, float& penetration) {
    AABBShape* aabbA = a->getShapeAs<AABBShape>();
    AABBShape* aabbB = b->getShapeAs<AABBShape>();

    float halfWA = aabbA->width / 2.f, halfHA = aabbA->height / 2.f;
    float halfWB = aabbB->width / 2.f, halfHB = aabbB->height / 2.f;

    float overlapX = (halfWA + halfWB) - std::abs(a->position.x - b->position.x);
    float overlapY = (halfHA + halfHB) - std::abs(a->position.y - b->position.y);

    if (overlapX > 0.f && overlapY > 0.f) {
        if (overlapX < overlapY) {
            normal = sf::Vector2f(a->position.x < b->position.x ? -1.f : 1.f, 0.f);
            penetration = overlapX;
        }
        else {
            normal = sf::Vector2f(0.f, a->position.y < b->position.y ? -1.f : 1.f);
            penetration = overlapY;
        }
        return true;
    }

    return false;
}

bool PhysicsWorld::checkAABBPlane(RigidBody* aabb, RigidBody* plane,
    sf::Vector2f& normal, float& penetration) {
    AABBShape* aabbShape = aabb->getShapeAs<AABBShape>();
    PlaneShape* planeShape = plane->getShapeAs<PlaneShape>();

    float halfW = aabbShape->width / 2.f;
    float halfH = aabbShape->height / 2.f;

    float projectedExtent = halfW * std::abs(planeShape->normal.x) +
        halfH * std::abs(planeShape->normal.y);

    sf::Vector2f toCenter = aabb->position - planeShape->point;
    float signedDist = Vec2::dot(toCenter, planeShape->normal);

    if (signedDist > 0.f && signedDist < projectedExtent) {
        normal = -planeShape->normal;
        penetration = projectedExtent - signedDist;
        return true;
    }

    return false;
}

void PhysicsWorld::resolveCollision(RigidBody* a, RigidBody* b,
    const sf::Vector2f& normal, float penetration) {
    float totalInvMass = a->getInverseMass() + b->getInverseMass();
    if (totalInvMass == 0.f) return; 

    float percent = POSITION_CORRECTION_PERCENT;
    float slop = POSITION_CORRECTION_SLOP;
    sf::Vector2f correction = normal *
        (std::max(penetration - slop, 0.f) / totalInvMass) * percent;

    a->position -= correction * a->getInverseMass();
    b->position += correction * b->getInverseMass();

    sf::Vector2f relVel = b->velocity - a->velocity;
    float velAlongNormal = Vec2::dot(relVel, normal);

    if (velAlongNormal > 0.f) return;

    float e = std::min(a->restitution, b->restitution);
    float j = -(1.f + e) * velAlongNormal / totalInvMass;

    sf::Vector2f impulse = normal * j;
    a->velocity -= impulse * a->getInverseMass();
    b->velocity += impulse * b->getInverseMass();
}