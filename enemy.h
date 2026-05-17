#include <SFML/Graphics.hpp>
#include "tank.h"
#include <string>
#include <cmath>
#include <cstdlib>


class EnemyTank : public Tank {

public:
    enum enemyState_ {
        PATROL,
        CHASE,
        ATTACK
    };

    enemyState_ currentState = PATROL;

    sf::Vector2f potrolTarget;
    bool patrolTargetSet = false;

    EnemyTank(std::string filename, sf::Vector2f size, float speed, int hp=100, bool alive=true) : Tank(filename, size, speed, hp, alive) {}

    float checkDistance(sf::Vector2f playerPostion) {
        float dx = playerPostion.x - this->getPosition().x;
        float dy = playerPostion.y - this->getPosition().y;
        return std::hypot(dx, dy);
    }

    void defaultPatrol() {
        int randomX = rand() % 201 - 100;
        int randomY = rand() % 201 - 100;
        sf::Vector2f e = sf::Vector2f(this->getPosition().x, this->getPosition().y);

        if (!patrolTargetSet) {
            potrolTarget = e;
            patrolTargetSet = true;
        }

        float dx = potrolTarget.x - e.x;
        float dy = potrolTarget.y - e.y;

        float d = std::hypot(dx, dy);

        if(d < 20) {
            potrolTarget = sf::Vector2f(e.x + randomX, e.y + randomY);
        }

        sf::Vector2f direction(
            potrolTarget.x - this->getPosition().x,
            potrolTarget.y - this->getPosition().y
        );

        float length = std::hypot(direction.x, direction.y);

        if(length > 0) {
            direction.x /= length;
            direction.y /= length;
        }

        velocity.x = direction.x * speed;
        velocity.y = direction.y * speed;

    }

    void moveToPlayer(int radius, sf::Vector2f playerPostion) {
        float distance = checkDistance(playerPostion);
        int stopDiatnse = 200;
        
        if (distance < radius) {
            if (distance > stopDiatnse)
            {
            
                sf::Vector2f direction(
                    playerPostion.x - this->getPosition().x,
                    playerPostion.y - this->getPosition().y
                );

                float length = std::hypot(direction.x, direction.y);

                if(length > 0) {
                    direction.x /= length;
                    direction.y /= length;
                }

                velocity.x = direction.x * speed;
                velocity.y = direction.y * speed;
            } else {
                velocity = sf::Vector2f(0.f ,0.f);
            }

        } else{
            velocity = sf::Vector2f(0.f ,0.f);
        }
    }

    sf::Vector2f shootAtPlayer(sf::Vector2f playerPostion) const {

        sf::Vector2f direction(
            playerPostion.x - this->getPosition().x,
            playerPostion.y - this->getPosition().y
        );

        float length = std::hypot(direction.x, direction.y);

        if(length > 0) {
            direction.x /= length;
            direction.y /= length;
        }
        
        return direction;
    }

    enemyState_ getState() {
        return currentState;
    }

    bool isAttacking() const {
        return currentState == ATTACK;
    }

    bool isChasing() const {
        return currentState == CHASE;
    }

    void updateAI(int radius, sf::Vector2f playerPostion) {
        float d = checkDistance(playerPostion);

        if(200 <= d && d < 400) {
            currentState = CHASE;
        } 

        if (d >= 400)
        {
            currentState = PATROL;
        }

        if(d < 200) {
            currentState = ATTACK;
        }

        switch (currentState)
        {
        case PATROL:
            defaultPatrol();
            break;

        case CHASE:
            moveToPlayer(radius, playerPostion);
            break;

        case ATTACK:
            // shootAtPlayer(playerPostion);
            velocity = sf::Vector2f(0.f ,0.f);
            break;
        
        default:
            break;
        }

    }




};