#include <SFML/Graphics.hpp>
#include "PhysicsWorld.h"
#include "Vector2.h"
#include <iostream>

int main() {

    sf::RenderWindow window(sf::VideoMode({ 1200, 700 }), "Pool Physics Simulation");
    window.setFramerateLimit(60);

    PhysicsWorld world;

    const float tableX = 100.f;
    const float tableY = 100.f;
    const float tableWidth = 1000.f;
    const float tableHeight = 500.f;
    const float cushionRadius = 15.f;

    for (float x = tableX; x <= tableX + tableWidth; x += cushionRadius * 2) {
        RigidBody* cushion = new RigidBody(
            sf::Vector2f(x, tableY),
            1.0f,
            cushionRadius,
            true
        );
        cushion->color = sf::Color(139, 69, 19);
        world.addBody(cushion);
    }

    for (float x = tableX; x <= tableX + tableWidth; x += cushionRadius * 2) {
        RigidBody* cushion = new RigidBody(
            sf::Vector2f(x, tableY + tableHeight),
            1.0f,
            cushionRadius,
            true
        );
        cushion->color = sf::Color(139, 69, 19);
        world.addBody(cushion);
    }

    for (float y = tableY; y <= tableY + tableHeight; y += cushionRadius * 2) {
        RigidBody* cushion = new RigidBody(
            sf::Vector2f(tableX, y),
            1.0f,
            cushionRadius,
            true
        );
        cushion->color = sf::Color(139, 69, 19);
        world.addBody(cushion);
    }

    for (float y = tableY; y <= tableY + tableHeight; y += cushionRadius * 2) {
        RigidBody* cushion = new RigidBody(
            sf::Vector2f(tableX + tableWidth, y),
            1.0f,
            cushionRadius,
            true
        );
        cushion->color = sf::Color(139, 69, 19);
        world.addBody(cushion);
    }

    RigidBody* cueBall = new RigidBody(
        sf::Vector2f(tableX + 200.f, tableY + tableHeight / 2),
        1.0f,
        15.f,
        false
    );
    cueBall->color = sf::Color::White;
    world.addBody(cueBall);

    float startX = tableX + tableWidth - 300.f; 
    float startY = tableY + tableHeight / 2;
    float ballRadius = 15.f;
    float spacing = ballRadius * 2 + 1.f;

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

            RigidBody* ball = new RigidBody(sf::Vector2f(x, y), 1.0f, ballRadius, false);
            ball->color = ballColors[colorIndex % 9];
            world.addBody(ball);

            colorIndex++;
        }
    }

    bool isDragging = false;
    sf::Vector2f dragStart;
    sf::Vector2f dragEnd;

    sf::Clock clock;

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos(mousePressed->position.x, mousePressed->position.y);

                    float dist = Vec2::distance(mousePos, cueBall->position);
                    if (dist < 100.f) {
                        isDragging = true;
                        dragStart = mousePos;
                        dragEnd = mousePos;
                    }
                }
            }

            if (event->is<sf::Event::MouseButtonReleased>()) {
                if (isDragging) {
                    isDragging = false;

                    sf::Vector2f force = (dragStart - dragEnd) * 20.f;
                    cueBall->applyImpulse(force);
                }
            }
        }

        if (isDragging) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            dragEnd = sf::Vector2f(mousePos.x, mousePos.y);
        }

        float dt = clock.restart().asSeconds();
        dt = std::min(dt, 0.02f); 
        world.step(dt);

        window.clear(sf::Color(20, 20, 20)); 

        sf::RectangleShape tableFelt({ tableWidth, tableHeight });
        tableFelt.setPosition({ tableX, tableY });
        tableFelt.setFillColor(sf::Color(0, 150, 0)); 
        window.draw(tableFelt);

        for (const auto* body : world.getBodies()) {
            sf::CircleShape circle(body->radius);
            circle.setPosition({ body->position.x - body->radius, body->position.y - body->radius });
            circle.setFillColor(body->color);

            circle.setOutlineThickness(2.f);
            circle.setOutlineColor(sf::Color::Black);

            window.draw(circle);
        }

        if (isDragging) {
            sf::Vertex line[] = {
                sf::Vertex(dragStart, sf::Color::White),
                sf::Vertex(dragEnd, sf::Color::White)
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