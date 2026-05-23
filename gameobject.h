#pragma once
#include <SFML/Graphics.hpp>

// Base interface for all game objects that update and draw.
class GameObject {
public:
    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;

    virtual ~GameObject() = default;
};
