#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include "map.h"
#include "tank.h"
#include "enemy.h"
#include "bullet.h"
#include "menu.h"

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
    EnemyTank enemy2("tank.png", sf::Vector2f(64.f, 64.f), 70.f, 100, true);
    enemy.initialize();
    enemy2.initialize();

    const sf::Vector2f playerSpawn((kMapLeft + kMapRight) * 0.5f,
                                   (kMapTop + kMapBottom) * 0.5f);
    const sf::Vector2f enemySpawn(kMapLeft + 64.f, kMapTop + 64.f);
    const sf::Vector2f enemy2Spawn(kMapRight - 64.f, kMapBottom - 64.f);
    player.respawn(playerSpawn);
    enemy.respawn(enemySpawn);
    enemy2.respawn(enemy2Spawn);

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

                if(enter) {
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

                if(enter) {
                    menu.currentState = Menu::PLAY;
                }
                break;
            }

            case Menu::PLAY: {
                winnerPlayed = false;
                if(music.getStatus() != sf::Music::Status::Playing) {
                    music.play();
                }
                if(!player.isAlive()) {
                    menu.currentState = Menu::MENU;
                    player.respawn(playerSpawn);
                    enemy.respawn(enemySpawn);
                    enemy2.respawn(enemy2Spawn);
                    bullets.clear();
                }

                if (!enemy.isAlive() && !enemy2.isAlive()) {
                    menu.currentState = Menu::WIN;
                    player.respawn(playerSpawn);
                    enemy.respawn(enemySpawn);
                    enemy2.respawn(enemy2Spawn);
                    bullets.clear();
                }
                
                const bool spaceDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
                if (spaceDown && !spaceWasDown &&
                    shootClock.getElapsedTime() >= sf::milliseconds(3)) {
                    sf::Vector2f pos = player.getPosition();

                    bullets.emplace_back(pos, player.fire(), BulletOwner::Player);
                    std::cout << bullets.size() << std::endl;
                    shootClock.restart();
                    if(!shootPlayed) {
                        shoot.play();
                    }
                }
                spaceWasDown = spaceDown;

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
                    if (player.isAlive() &&
                        (enemy.isAttacking() || enemy.isChasing()) &&
                        enemyShootClock.getElapsedTime() >= enemyShootDelay) {
                        sf::Vector2f bulletDirection =
                            enemy.shootAtPlayer(player.getPosition());

                        bullets.emplace_back(enemy.getPosition(), bulletDirection,
                                             BulletOwner::Enemy);
                        enemyShootClock.restart();
                    }
                } else {
                    enemy.velocity = sf::Vector2f(0.f, 0.f);
                }

                if (enemy2.isAlive()) {
                    enemy2.updateAI(600, player.getPosition());
                    if (player.isAlive() &&
                        (enemy2.isAttacking() || enemy2.isChasing()) &&
                        enemyShootClock.getElapsedTime() >= enemyShootDelay) {
                        sf::Vector2f bulletDirection =
                            enemy2.shootAtPlayer(player.getPosition());

                        bullets.emplace_back(enemy2.getPosition(), bulletDirection,
                                             BulletOwner::Enemy);
                        enemyShootClock.restart();
                    }
                } else {
                    enemy2.velocity = sf::Vector2f(0.f, 0.f);
                }

                sf::Vector2f oldPosition = enemy.getPosition();
                sf::Vector2f oldPosition2 = enemy2.getPosition();

                enemy.update(dt);
                clampToBounds(enemy);

                enemy2.update(dt);
                clampToBounds(enemy2);

                if (enemy.bounds().findIntersection(player.bounds())) {
                    enemy.velocity = sf::Vector2f(0.f, 0.f);
                    enemy.setPosition(oldPosition);
                    clampToBounds(enemy);
                }

                if (enemy2.bounds().findIntersection(player.bounds())) {
                    enemy2.velocity = sf::Vector2f(0.f, 0.f);
                    enemy2.setPosition(oldPosition2);
                    clampToBounds(enemy2);
                }

                if (enemy.isAlive()) {
                    window.draw(enemy);
                }

                if (enemy2.isAlive()) {
                    window.draw(enemy2);
                }

                for (int i = 0; i < bullets.size(); i++) {
                    if (bullets[i].isAlive()) {
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
                        if (bullets[i].getOwner() == BulletOwner::Player &&
                            bullets[i].bounds().findIntersection(enemy.bounds())) {
                            enemy.takeDamage(50);
                            bullets[i].destroy();
                            if (!hitPlayed) {
                                hit.play();
                            }
                        }

                        if (bullets[i].getOwner() == BulletOwner::Player &&
                            bullets[i].bounds().findIntersection(enemy2.bounds())) {
                            enemy2.takeDamage(50);
                            bullets[i].destroy();
                            if (!hitPlayed) {
                                hit.play();
                            }
                        }

                        if (player.isAlive() &&
                            bullets[i].getOwner() == BulletOwner::Enemy &&
                            bullets[i].bounds().findIntersection(player.bounds())) {
                            player.takeDamage(50);
                            bullets[i].destroy();
                        }
                    }
                }

                auto it = std::remove_if(
                    bullets.begin(), bullets.end(),
                    [](const Bullet& bullet) { return !bullet.isAlive(); });
                bullets.erase(it, bullets.end());

                for (int i = 0; i < bullets.size(); i++) {
                    if (bullets[i].isAlive()) {
                        bullets[i].draw(window);
                    }
                }

                break;
            }
            default: {
                break;
            }
        }

        window.display();
    }

    return 0;
}
