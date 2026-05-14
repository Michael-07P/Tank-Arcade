#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

class Tank : public RectangleShape
{
protected:
    string filename;
    float width;
    float height;
    float speed;
    Texture texture;

public:

    Tank(string filename, Vector2f size, float speed) 
        : RectangleShape(size), filename(filename), width(size.x), height(size.y), speed(speed) {}

    sf::Vector2f velocity = sf::Vector2f(0.f, 0.f);

    void initialize() {
        if(!texture.loadFromFile(filename)){
            cout << "Error while loading assets";
        }
        setSize(Vector2f(static_cast<float>(texture.getSize().x), static_cast<float>(texture.getSize().y)));
        setTexture(&texture);
    }

    void handleInput()
    {
        velocity = sf::Vector2f(0.f, 0.f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            velocity.y = -speed;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            velocity.y = speed;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            velocity.x = -speed;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            velocity.x = speed;
    }

    void update(float dt)
    {
        move(sf::Vector2f(velocity.x * dt, velocity.y * dt));
    }
};