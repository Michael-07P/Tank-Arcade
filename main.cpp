#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <optional>
#include "map.h"
#include "tank.h"
#include "bullet.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Tank Arcade");

    Map map;
    Tank player("tank.png", sf::Vector2f(64.f, 64.f), 100.f);
    player.initialize();

    std::vector<Bullet> bullets;
    bullets.reserve(256);

    sf::Clock shootClock;
    bool spaceWasDown = false;

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        const bool spaceDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
        if (spaceDown && !spaceWasDown && shootClock.getElapsedTime() >= sf::milliseconds(3)) {
            sf::Vector2f pos = player.getPosition();

            bullets.emplace_back(pos, player.fire());
            std::cout << bullets.size() << std::endl;
            shootClock.restart();
        }
        spaceWasDown = spaceDown;

        window.clear(sf::Color::Black);

        
        map.draw(window);
        player.handleInput();

        player.update(dt);
        window.draw(player);

        for (int i = 0; i < bullets.size(); i++) {
            bullets[i].update(dt);
        }

        for (int i = 0; i < bullets.size(); i++) {
            bullets[i].draw(window);
        }


        window.display();
    }

    return 0;
}
