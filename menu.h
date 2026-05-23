#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

using namespace sf;

// Represents simple menu state for the game.
class Menu {

public:
    enum gameState {
        MENU,
        SETTINGS,
        PLAY,
        WIN
    };

    gameState currentState = MENU; 

private:
    sf::Font font;

public:
    void update(sf::RenderWindow & window) {
        switch (currentState) {
            case MENU:
                break;

            case SETTINGS:
                break;

            case PLAY:
                
                break;

            case WIN:
                
                break;
            
            default:
                break;
        };
    }



     


};