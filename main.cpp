#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "map.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "My window");

    Map map;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::Black);

        map.draw(window);

        // tank.handleInput();
        // tank.update(0.016f);  // ~60 FPS
        // window.draw(tank);

        window.display();
    }

    return 0;
}
