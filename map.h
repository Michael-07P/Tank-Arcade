#include <vector>
#include <SFML/Graphics.hpp>
#include <iostream>

// Represents a tile-based map for the level.
class Map {
private:
    std::vector<std::vector<int>> grid; // 2D grid representing the map (0 for empty, 1 for wall, etc.)
    sf::Texture sand;
    sf::Texture red;
    const int tileSize = 64;
public:
    Map() {
        // creating a simple grid for demonstration    
        // grid = {
        //     {0,0,0,0},
        //     {0,1,1,0},
        //     {0,1,1,0},
        //     {0,0,0,0}
        // };

        grid.resize(11, std::vector<int>(20, 1));

        for(int x = 0; x < 20; x++) {
            grid[0][x] = 0;
            grid[10][x] = 0;
        }

        for(int y = 0; y < 11; y++) {
            grid[y][0] = 0;
            grid[y][19] = 0;
        }

        if (!red.loadFromFile("red.png"))
        std::cout << "RED NOT LOADED\n";
        else {
            auto s = red.getSize();
            std::cout << "RED loaded (" << s.x << "x" << s.y << ")\n";
        }

        if (!sand.loadFromFile("sandbagBeige.png"))
        std::cout << "SAND NOT LOADED\n";
        else {
            auto s = sand.getSize();
            std::cout << "SAND loaded (" << s.x << "x" << s.y << ")\n";
        }
        // red.loadFromFile("./assets/red.png");
        // sand.loadFromFile("./assets/sandbagBeige.png");
    }

    void draw(sf::RenderWindow& window) {
        for (size_t y = 0; y < grid.size(); ++y) {
            for (size_t x = 0; x < grid[y].size(); ++x) {
                sf::Vector2f pos(static_cast<float>(x * tileSize), static_cast<float>(y * tileSize));
                const sf::Texture& tileTexture = (grid[y][x] == 1) ? sand : red;
                sf::Sprite tile(tileTexture);

                // Scale sprite to the tileSize so it's visible on large windows
                sf::Vector2u texSize = tileTexture.getSize();
                if (texSize.x > 0 && texSize.y > 0) {
                    float sx = static_cast<float>(tileSize) / static_cast<float>(texSize.x);
                    float sy = static_cast<float>(tileSize) / static_cast<float>(texSize.y);
                    tile.setScale({sx, sy});
                }

                tile.setPosition(pos);
                window.draw(tile);
            }
        }
    }

    
};