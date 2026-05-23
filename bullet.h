#include <SFML/Graphics.hpp>
#include <iostream>
#include "gameobject.h"

enum class BulletOwner {
    Player,
    Enemy
};

// Represents a projectile fired by player or enemy tanks.
class Bullet : public GameObject {

protected:
    sf::Vector2f position;
    sf::Vector2f direction;
    float speed = 200.f;
    int damage = 50;
    bool alive;
    BulletOwner owner;
    static sf::Texture& sharedTexture() {
        static sf::Texture texture;
        static bool initialized = false;

        if (!initialized) {
            initialized = true;
            if (!texture.loadFromFile("bullet.png")) {
                std::cout << "BULLET NOT LOADED\n";
            }
        }

        return texture;
    }

    sf::Sprite bullet;

public:
    Bullet(sf::Vector2f position, sf::Vector2f direction, BulletOwner owner = BulletOwner::Player, bool alive=true)
        : position(position), direction(direction), alive(alive), owner(owner), bullet(sharedTexture()) {
        bullet.setPosition(position);
    };


    void update(float dt) override {
        position.x += direction.x * speed * dt;
        position.y += direction.y * speed * dt;
        bullet.setPosition(position);
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(bullet);
    }

    void destroy() {
        alive = false;
    }

    bool  isAlive() const {
        return alive;
    }

    BulletOwner getOwner() const {
        return owner;
    }

    sf::FloatRect bounds() {
        return bullet.getGlobalBounds();
    }
};