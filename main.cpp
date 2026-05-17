#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include "map.h"
#include "tank.h"
#include "enemy.h"
#include "bullet.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Tank Arcade");

    const float kMapLeft = 32.f;
    const float kMapRight = 1280.f - 32.f;
    const float kMapTop = 32.f;
    const float kMapBottom = 720.f - 32.f;

    Map map;
    Tank player("tank.png", sf::Vector2f(64.f, 64.f), 100.f);
    player.initialize();

    EnemyTank enemy("tank.png", sf::Vector2f(64.f, 64.f), 80.f, 100, true);
    enemy.initialize();

    const sf::Vector2f playerSpawn((kMapLeft + kMapRight) * 0.5f, (kMapTop + kMapBottom) * 0.5f);
    const sf::Vector2f enemySpawn(kMapLeft + 64.f, kMapTop + 64.f);
    player.setPosition(playerSpawn);
    enemy.setPosition(enemySpawn);


    std::vector<Bullet> bullets;
    bullets.reserve(256);

    sf::Clock shootClock;
    bool spaceWasDown = false;

    sf::Clock enemyShootClock;
    const sf::Time enemyShootDelay = sf::milliseconds(600);

    sf::Clock clock;

    auto clampToBounds = [&](sf::Transformable& obj) {
        sf::Vector2f pos = obj.getPosition();
        pos.x = std::max(kMapLeft, std::min(pos.x, kMapRight));
        pos.y = std::max(kMapTop, std::min(pos.y, kMapBottom));
        obj.setPosition(pos);
    };

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        const bool spaceDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
        if (spaceDown && !spaceWasDown && shootClock.getElapsedTime() >= sf::milliseconds(3)) {
            sf::Vector2f pos = player.getPosition();

            bullets.emplace_back(pos, player.fire(), BulletOwner::Player);
            std::cout << bullets.size() << std::endl;
            shootClock.restart();
        }
        spaceWasDown = spaceDown;

        window.clear(sf::Color::Black);

        map.draw(window);

        if (player.isAlive()) {
            player.handleInput();
            player.update(dt);
            clampToBounds(player);
            window.draw(player);
        }

        if (enemy.isAlive()) {
            enemy.updateAI(600, player.getPosition());
                // Shoot in ATTACK and CHASE so the enemy fires while moving.
                if(player.isAlive() && (enemy.isAttacking() || enemy.isChasing()) &&
               enemyShootClock.getElapsedTime() >= enemyShootDelay) {
                sf::Vector2f bulletDirection = enemy.shootAtPlayer(player.getPosition());

                bullets.emplace_back(enemy.getPosition(), bulletDirection, BulletOwner::Enemy);
                enemyShootClock.restart();
            }
        } else {
            enemy.velocity = sf::Vector2f(0.f, 0.f);
        }

        sf::Vector2f oldPosition = enemy.getPosition();

        enemy.update(dt);
        clampToBounds(enemy);
        
        if (enemy.bounds().findIntersection(player.bounds()))
        {
            enemy.velocity = sf::Vector2f(0.f ,0.f);
            enemy.setPosition(oldPosition);
            clampToBounds(enemy);
        }
        

        if(enemy.isAlive()) {
            window.draw(enemy);
        }
        
        for (int i = 0; i < bullets.size(); i++) {
            if(bullets[i].isAlive()) {
                bullets[i].update(dt);

                sf::FloatRect b = bullets[i].bounds();
                // SFML 3 Rect uses position/size instead of left/top/width/height.
                const float left = b.position.x;
                const float top = b.position.y;
                const float right = b.position.x + b.size.x;
                const float bottom = b.position.y + b.size.y;
                if (left > kMapRight || right < kMapLeft ||
                    top > kMapBottom || bottom < kMapTop) {
                    bullets[i].destroy();
                }

                // Only player bullets can damage the enemy.
                if(bullets[i].getOwner() == BulletOwner::Player && bullets[i].bounds().findIntersection(enemy.bounds())) {
                    enemy.takeDamage(50);
                    bullets[i].destroy();
                }

                if(player.isAlive() && bullets[i].getOwner() == BulletOwner::Enemy && bullets[i].bounds().findIntersection(player.bounds())) {
                    player.takeDamage(50);
                    bullets[i].destroy();
                }
            }
            

            
        }

        auto it = std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& bullet) {
            return !bullet.isAlive();
        });
        bullets.erase(it, bullets.end());

        for (int i = 0; i < bullets.size(); i++) {
            if(bullets[i].isAlive()) {
                bullets[i].draw(window);
            }
           
        }

        window.display();
    }

    return 0;
}
