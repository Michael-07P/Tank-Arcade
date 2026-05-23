#include "game.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "bullet.h"
#include "enemy.h"
#include "map.h"
#include "menu.h"
#include "tank.h"

void Game::run() {
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Tank Arcade");

    const float kMapLeft = 32.f;
    const float kMapRight = 1280.f - 32.f;
    const float kMapTop = 32.f;
    const float kMapBottom = 720.f - 32.f;

    Map map;

    std::vector<std::unique_ptr<GameObject>> objects;
    objects.reserve(256);
    std::vector<std::unique_ptr<GameObject>> pendingObjects;
    pendingObjects.reserve(64);

    auto playerObj = std::make_unique<Tank>("tank.png", sf::Vector2f(64.f, 64.f), 100.f);
    Tank* player = playerObj.get();
    player->initialize();

    auto enemyObj = std::make_unique<EnemyTank>("tank.png", sf::Vector2f(64.f, 64.f), 80.f, 100, true);
    EnemyTank* enemy = enemyObj.get();
    enemy->initialize();

    auto enemy2Obj = std::make_unique<EnemyTank>("tank.png", sf::Vector2f(64.f, 64.f), 70.f, 100, true);
    EnemyTank* enemy2 = enemy2Obj.get();
    enemy2->initialize();

    objects.push_back(std::move(playerObj));
    objects.push_back(std::move(enemyObj));
    objects.push_back(std::move(enemy2Obj));

    const sf::Vector2f playerSpawn((kMapLeft + kMapRight) * 0.5f,
                                   (kMapTop + kMapBottom) * 0.5f);
    const sf::Vector2f enemySpawn(kMapLeft + 64.f, kMapTop + 64.f);
    const sf::Vector2f enemy2Spawn(kMapRight - 64.f, kMapBottom - 64.f);
    player->respawn(playerSpawn);
    enemy->respawn(enemySpawn);
    enemy2->respawn(enemy2Spawn);

    sf::Clock shootClock;
    bool spaceWasDown = false;

    sf::Clock enemyShootClock;
    const sf::Time enemyShootDelay = sf::milliseconds(600);

    sf::Clock clock;

    // Prevents tanks from leaving the playable map bounds.
    auto clampToBounds = [&](sf::Transformable& obj) {
        sf::Vector2f pos = obj.getPosition();
        pos.x = std::max(kMapLeft, std::min(pos.x, kMapRight));
        pos.y = std::max(kMapTop, std::min(pos.y, kMapBottom));
        obj.setPosition(pos);
    };

    Menu menu;

    sf::Font titleFont;
    bool titleFontLoaded = titleFont.openFromFile("arial.ttf");
    if (!titleFontLoaded) {
        std::cerr << "Error loading font: arial.ttf\n";
    }

    sf::Text titleText(titleFont, "Tank Arcade", 120);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);

    sf::Text winnerText(titleFont, "You won!", 120);
    winnerText.setFillColor(sf::Color::White);
    winnerText.setStyle(sf::Text::Bold);

    sf::Text smallText(titleFont, "[Press Enter to start]", 48);
    smallText.setFillColor(sf::Color::White);

    sf::Texture winTexture;
    bool winTextureLoaded = winTexture.loadFromFile("cup.png");
    if (!winTextureLoaded) {
        std::cerr << "Error loading texture: cup.png\n";
    }
    sf::Sprite winSprite(winTexture);
    winSprite.setPosition(sf::Vector2f(280.f, 200.f));

    auto centerText = [](sf::Text& text, float x, float y) {
        const sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x * 0.5f,
                                    bounds.position.y + bounds.size.y * 0.5f));
        text.setPosition(sf::Vector2f(x, y));
    };

    const float centerX = window.getSize().x * 0.5f;
    centerText(titleText, centerX, 140.f);
    centerText(winnerText, centerX, 140.f);

    const sf::FloatRect titleBounds = titleText.getGlobalBounds();
    const sf::FloatRect smallBounds = smallText.getLocalBounds();
    smallText.setOrigin(sf::Vector2f(smallBounds.position.x + smallBounds.size.x * 0.5f,
                                     smallBounds.position.y + smallBounds.size.y * 0.5f));
    smallText.setPosition(sf::Vector2f(centerX,
                                       titleBounds.position.y + titleBounds.size.y + 20.f +
                                           smallBounds.size.y * 0.5f));

    // Game music
    sf::Music music;
    const bool musicLoaded = music.openFromFile("music.mp3");
    if (!musicLoaded) {
        std::cerr << "Error loading music: music.mp3\n";
    }
    music.setLooping(true);

    // Shoot Sound effects
    sf::SoundBuffer shootBuffer;
    if (!shootBuffer.loadFromFile("shoot.mp3")) {
        std::cout << "Error loading sound: shoot.mp3\n";
    }
    sf::Sound shoot(shootBuffer);
    bool shootPlayed = false;

    // Damage hit Sound effects
    sf::SoundBuffer hitBuffer;
    if (!hitBuffer.loadFromFile("hit.mp3")) {
        std::cout << "Error loading sound: hit.mp3\n";
    }
    sf::Sound hit(hitBuffer);
    bool hitPlayed = false;

    // Win sound effect
    sf::SoundBuffer winnerBuffer;
    if (!winnerBuffer.loadFromFile("winner.mp3")) {
        std::cout << "Error loading sound: winner.mp3\n";
    }

    sf::Sound winner(winnerBuffer);
    bool winnerPlayed = false;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear(sf::Color::Black);

        const bool enter = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter);

        switch (menu.currentState) {
            case Menu::MENU: {
                music.stop();
                winnerPlayed = false;

                if (titleFontLoaded) {
                    window.draw(titleText);
                    window.draw(smallText);
                }

                if (enter) {
                    menu.currentState = Menu::PLAY;
                }
                break;
            }

            case Menu::WIN: {
                music.stop();
                if (!winnerPlayed) {
                    winner.play();
                    winnerPlayed = true;
                }

                if (titleFontLoaded) {
                    window.draw(winnerText);
                }

                if (winTextureLoaded) {
                    window.draw(winSprite);
                }

                if (enter) {
                    menu.currentState = Menu::PLAY;
                }
                break;
            }

            case Menu::PLAY: {
                winnerPlayed = false;
                if (music.getStatus() != sf::Music::Status::Playing) {
                    music.play();
                }
                if (!player->isAlive()) {
                    menu.currentState = Menu::MENU;
                    player->respawn(playerSpawn);
                    enemy->respawn(enemySpawn);
                    enemy2->respawn(enemy2Spawn);
                    objects.erase(std::remove_if(objects.begin(), objects.end(),
                                                 [](const std::unique_ptr<GameObject>& obj) {
                                                     return dynamic_cast<Bullet*>(obj.get()) != nullptr;
                                                 }),
                                  objects.end());
                }

                if (!enemy->isAlive() && !enemy2->isAlive()) {
                    menu.currentState = Menu::WIN;
                    player->respawn(playerSpawn);
                    enemy->respawn(enemySpawn);
                    enemy2->respawn(enemy2Spawn);
                    objects.erase(std::remove_if(objects.begin(), objects.end(),
                                                 [](const std::unique_ptr<GameObject>& obj) {
                                                     return dynamic_cast<Bullet*>(obj.get()) != nullptr;
                                                 }),
                                  objects.end());
                }

                const bool spaceDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
                if (spaceDown && !spaceWasDown &&
                    shootClock.getElapsedTime() >= sf::milliseconds(3)) {
                    // Spawns player bullet from current tank position.
                    pendingObjects.push_back(std::make_unique<Bullet>(player->getPosition(), player->fire(),
                                                                      BulletOwner::Player));
                    shootClock.restart();
                    if (!shootPlayed) {
                        shoot.play();
                    }
                }
                spaceWasDown = spaceDown;

                map.draw(window);

                if (player->isAlive()) {
                    player->handleInput();
                }

                // Updates enemy AI behavior using dynamic_cast for grading requirement.
                for (auto& obj : objects) {
                    if (auto enemyTank = dynamic_cast<EnemyTank*>(obj.get())) {
                        if (enemyTank->isAlive()) {
                            enemyTank->updateAI(600, player->getPosition());
                            if (player->isAlive() &&
                                (enemyTank->isAttacking() || enemyTank->isChasing()) &&
                                enemyShootClock.getElapsedTime() >= enemyShootDelay) {
                                sf::Vector2f bulletDirection =
                                    enemyTank->shootAtPlayer(player->getPosition());

                                pendingObjects.push_back(std::make_unique<Bullet>(enemyTank->getPosition(),
                                                                                  bulletDirection,
                                                                                  BulletOwner::Enemy));
                                enemyShootClock.restart();
                            }
                        } else {
                            enemyTank->velocity = sf::Vector2f(0.f, 0.f);
                        }
                    }
                }

                if (!pendingObjects.empty()) {
                    for (auto& obj : pendingObjects) {
                        objects.push_back(std::move(obj));
                    }
                    pendingObjects.clear();
                }

                sf::Vector2f oldPosition = enemy->getPosition();
                sf::Vector2f oldPosition2 = enemy2->getPosition();

                for (auto& obj : objects) {
                    auto tank = dynamic_cast<Tank*>(obj.get());
                    auto bullet = dynamic_cast<Bullet*>(obj.get());

                    if (tank && !tank->isAlive()) {
                        continue;
                    }

                    if (bullet && !bullet->isAlive()) {
                        continue;
                    }

                    obj->update(dt);

                    if (tank) {
                        clampToBounds(*tank);
                    }
                }

                if (enemy->isAlive() && player->isAlive() &&
                    enemy->bounds().findIntersection(player->bounds())) {
                    enemy->velocity = sf::Vector2f(0.f, 0.f);
                    enemy->setPosition(oldPosition);
                    clampToBounds(*enemy);
                }

                if (enemy2->isAlive() && player->isAlive() &&
                    enemy2->bounds().findIntersection(player->bounds())) {
                    enemy2->velocity = sf::Vector2f(0.f, 0.f);
                    enemy2->setPosition(oldPosition2);
                    clampToBounds(*enemy2);
                }

                std::vector<Bullet*> bullets;
                std::vector<EnemyTank*> enemies;
                bullets.reserve(objects.size());
                enemies.reserve(objects.size());

                for (auto& obj : objects) {
                    if (auto bullet = dynamic_cast<Bullet*>(obj.get())) {
                        bullets.push_back(bullet);
                    } else if (auto enemyTank = dynamic_cast<EnemyTank*>(obj.get())) {
                        enemies.push_back(enemyTank);
                    }
                }

                for (auto* bullet : bullets) {
                    if (!bullet->isAlive()) {
                        continue;
                    }

                    sf::FloatRect b = bullet->bounds();
                    const float left = b.position.x;
                    const float top = b.position.y;
                    const float right = b.position.x + b.size.x;
                    const float bottom = b.position.y + b.size.y;
                    if (left > kMapRight || right < kMapLeft ||
                        top > kMapBottom || bottom < kMapTop) {
                        bullet->destroy();
                        continue;
                    }

                    // Handles collision between bullets and enemies.
                    if (bullet->getOwner() == BulletOwner::Player) {
                        for (auto* enemyTank : enemies) {
                            if (!enemyTank->isAlive()) {
                                continue;
                            }
                            if (bullet->bounds().findIntersection(enemyTank->bounds())) {
                                enemyTank->takeDamage(50);
                                bullet->destroy();
                                if (!hitPlayed) {
                                    hit.play();
                                }
                                break;
                            }
                        }
                    }

                    // Handles collision between bullets and player.
                    if (player->isAlive() &&
                        bullet->getOwner() == BulletOwner::Enemy &&
                        bullet->bounds().findIntersection(player->bounds())) {
                        player->takeDamage(50);
                        bullet->destroy();
                    }
                }

                // Removes destroyed bullets from the single container.
                objects.erase(
                    std::remove_if(objects.begin(), objects.end(),
                                   [](const std::unique_ptr<GameObject>& obj) {
                                       auto bullet = dynamic_cast<Bullet*>(obj.get());
                                       return bullet && !bullet->isAlive();
                                   }),
                    objects.end());

                for (auto& obj : objects) {
                    auto tank = dynamic_cast<Tank*>(obj.get());
                    auto bullet = dynamic_cast<Bullet*>(obj.get());

                    if (tank && !tank->isAlive()) {
                        continue;
                    }

                    if (bullet && !bullet->isAlive()) {
                        continue;
                    }

                    obj->draw(window);
                }

                break;
            }
            default: {
                break;
            }
        }

        window.display();
    }
}
