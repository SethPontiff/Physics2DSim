#include <SFML/Graphics.hpp>
#include "PhysicsWorld.h"
#include "Vector2.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({ 1200, 700 }), "Pool Physics Simulation");
    window.setFramerateLimit(60);

    PhysicsWorld world;

    //table dimensions
    const float tableX = 100.f;
    const float tableY = 100.f;
    const float tableWidth = 1000.f;
    const float tableHeight = 500.f;


    // Top wall
    RigidBody* topWall = new RigidBody(
        sf::Vector2f(tableX + tableWidth / 2.f, tableY),
        1.f,
        new PlaneShape(sf::Vector2f(0.f, 1.f), sf::Vector2f(0.f, tableY)),
        true
    );
    topWall->color = sf::Color(139, 69, 19);
    world.addBody(topWall);

    // Bottom wall
    RigidBody* bottomWall = new RigidBody(
        sf::Vector2f(tableX + tableWidth / 2.f, tableY + tableHeight),
        1.f,
        new PlaneShape(sf::Vector2f(0.f, -1.f), sf::Vector2f(0.f, tableY + tableHeight)),
        true
    );
    bottomWall->color = sf::Color(139, 69, 19);
    world.addBody(bottomWall);

    // Left wall
    RigidBody* leftWall = new RigidBody(
        sf::Vector2f(tableX, tableY + tableHeight / 2.f),
        1.f,
        new PlaneShape(sf::Vector2f(1.f, 0.f), sf::Vector2f(tableX, 0.f)),
        true
    );
    leftWall->color = sf::Color(139, 69, 19);
    world.addBody(leftWall);

    // Right wall
    RigidBody* rightWall = new RigidBody(
        sf::Vector2f(tableX + tableWidth, tableY + tableHeight / 2.f),
        1.f,
        new PlaneShape(sf::Vector2f(-1.f, 0.f), sf::Vector2f(tableX + tableWidth, 0.f)),
        true
    );
    rightWall->color = sf::Color(139, 69, 19);
    world.addBody(rightWall);

    // Cue ball
    RigidBody* cueBall = new RigidBody(
        sf::Vector2f(tableX + 200.f, tableY + tableHeight / 2.f),
        1.f,
        new CircleShape(15.f),
        false
    );
    cueBall->color = sf::Color::White;
    world.addBody(cueBall);

    // Rack of balls
    float startX = tableX + tableWidth - 300.f;
    float startY = tableY + tableHeight / 2.f;
    float ballRad = 15.f;
    float spacing = ballRad * 2.f + 1.f;

    sf::Color ballColors[] = {
        sf::Color::Yellow,
        sf::Color::Blue,
        sf::Color::Red,
        sf::Color(255, 165, 0),
        sf::Color::Green,
        sf::Color(128, 0, 128),
        sf::Color(139, 69, 19),
        sf::Color::Cyan,
        sf::Color::Magenta
    };

    int colorIndex = 0;
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col <= row; col++) {
            float x = startX + row * spacing * 0.866f;
            float y = startY + (col - row / 2.0f) * spacing;
            RigidBody* ball = new RigidBody(
                sf::Vector2f(x, y), 1.f, new CircleShape(ballRad), false
            );
            ball->color = ballColors[colorIndex % 9];
            world.addBody(ball);
            colorIndex++;
        }
    }

    // Cue mechanic
    bool isDragging = false;
    sf::Vector2f dragStart, dragEnd;

    // Fixed time step
    const float FIXED_TIME_STEP = 1.f / 60.f;
    float accumulator = 0.f;
    sf::Clock clock;

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* mp = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mp->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos(mp->position.x, mp->position.y);
                    if (Vec2::distance(mousePos, cueBall->position) < 100.f) {
                        isDragging = true;
                        dragStart = mousePos;
                        dragEnd = mousePos;
                    }
                }
            }

            if (event->is<sf::Event::MouseButtonReleased>()) {
                if (isDragging) {
                    isDragging = false;
                    sf::Vector2f impulse = (dragStart - dragEnd) * 20.f;
                    cueBall->applyImpulse(impulse);
                }
            }
        }

        if (isDragging) {
            sf::Vector2i mp = sf::Mouse::getPosition(window);
            dragEnd = sf::Vector2f(mp.x, mp.y);
        }

        // Fixed time step update
        float frameTime = clock.restart().asSeconds();
        frameTime = std::min(frameTime, 0.25f);
        accumulator += frameTime;

        while (accumulator >= FIXED_TIME_STEP) {
            world.step(FIXED_TIME_STEP);
            accumulator -= FIXED_TIME_STEP;
        }

        window.clear(sf::Color(20, 20, 20));

        sf::RectangleShape felt({ tableWidth, tableHeight });
        felt.setPosition({ tableX, tableY });
        felt.setFillColor(sf::Color(0, 150, 0));
        window.draw(felt);

        for (const auto* body : world.getBodies()) {
            ShapeType type = body->shape->getType();

            if (type == ShapeType::Circle) {
                CircleShape* cs = static_cast<CircleShape*>(body->shape);
                sf::CircleShape circle(cs->radius);
                circle.setPosition({ body->position.x - cs->radius,
                                    body->position.y - cs->radius });
                circle.setFillColor(body->color);
                circle.setOutlineThickness(2.f);
                circle.setOutlineColor(sf::Color::Black);
                window.draw(circle);
            }
            else if (type == ShapeType::Plane) {
                PlaneShape* ps = static_cast<PlaneShape*>(body->shape);
                sf::RectangleShape line;

                if (std::abs(ps->normal.x) > std::abs(ps->normal.y)) {
                    line.setSize({ 10.f, tableHeight });
                    line.setPosition({ ps->point.x - 5.f, tableY });
                }
                else {
                    line.setSize({ tableWidth, 10.f });
                    line.setPosition({ tableX, ps->point.y - 5.f });
                }

                line.setFillColor(body->color);
                window.draw(line);
            }
        }

        // cue line/power indicator.
        if (isDragging) {
            sf::Vertex line[] = {
                sf::Vertex(dragStart, sf::Color::White),
                sf::Vertex(dragEnd,   sf::Color::White)
            };
            window.draw(line, 2, sf::PrimitiveType::Lines);

            float power = Vec2::length(dragEnd - dragStart);
            sf::CircleShape powerCircle(5.f + power * 0.2f);
            powerCircle.setPosition({ dragStart.x - powerCircle.getRadius(),
                                     dragStart.y - powerCircle.getRadius() });
            powerCircle.setFillColor(sf::Color(255, 255, 0, 128));
            window.draw(powerCircle);
        }

        window.display();
    }

    return 0;
}