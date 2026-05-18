#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

class Tank : public RectangleShape
{

public:
    sf::Vector2f velocity;

protected:
    string filename;
    float width;
    float height;
    float speed;
    Texture texture;
    int hp;
    bool alive;

public:

    Tank(string filename, Vector2f size, float speed, int hp=100, bool alive=true) 
        : RectangleShape(size), filename(filename), width(size.x), height(size.y), speed(speed), velocity(0.f, 0.f), hp(hp), alive(alive) {}


    void initialize() {
        if(!texture.loadFromFile(filename)){
            cout << "Error while loading assets";
        }
        setTexture(&texture);
        setSize({width, height});
        setOrigin({
            width / 2.f,
            height / 2.f
        });
    }

    void handleInput()
    {
        velocity = sf::Vector2f(0.f, 0.f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
            velocity.y = -speed;
            setRotation(sf::degrees(0.f));
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
            velocity.y = speed;
            setRotation(sf::degrees(180.f));
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            velocity.x = -speed;
            setRotation(sf::degrees(270.f));
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            velocity.x = speed;
            setRotation(sf::degrees(90.f));
        }
            
    }

    Vector2f fire() {
        float rotation = getRotation().asDegrees();

        if (rotation == 0.f) {
            return {0.f, -1.f};
        }

        if (rotation == 90.f) {
            return {1.f, 0.f};
        }

        if (rotation == 180.f) {
            return {0.f, 1.f};
        }

        if (rotation == 270.f) {
            return {-1.f, 0.f};
        }

        return {0.f, -1.f}; 
    }

    void update(float dt)
    {
        move(sf::Vector2f(velocity.x * dt, velocity.y * dt));
    }

    void takeDamage(int damage) {
        hp -= damage;

        if(hp <= 0) {
            alive = false;
        }
    }

    bool isAlive() const {
        return alive;
    }

    sf::FloatRect bounds() {
        return this->getGlobalBounds();
    }

    void respawn(sf::Vector2f spawnPosition) {
        alive = true;
        hp = 100;
        setPosition(spawnPosition);
        velocity = sf::Vector2f(0.f, 0.f);
    }
};