#ifndef SNAKE_H
#define SNAKE_H

#include <vector>

enum class Direction
{
    Up = 0,
    Down = 1,
    Left = 2,
    Right = 3,
};

enum class OBJ
{
    nothing = 0,
    food = 1,
    portal = 2,
    body = 3,
    wall = 4,
    obstacle = 5,
};

class SnakeBody
{
public:
    SnakeBody();
    SnakeBody(int x, int y);
    int getX() const;
    int getY() const;
    bool operator == (const SnakeBody& snakeBody);
private:
    int mX;
    int mY;
};

// Snake class should have no depency on the GUI library
class Snake
{
public:
    //Snake();
    Snake(int gameBoardWidth, int gameBoardHeight, int initialSnakeLength);
    // Set random seed
    void setRandomSeed();
    // Initialize snake
    void initializeSnake();
    // Checking API for generating random food
    bool isPartOfSnake(int x, int y);
    void senseFood(SnakeBody food);
    bool touchFood();
    // Check if the snake is dead
    bool hitWall();
    bool hitSelf();

    bool changeDirection(Direction newDirection);
    std::vector<SnakeBody>& getSnake();
    int getLength();
    SnakeBody createNewHead();
    OBJ moveFoward();

    void sensePortal(SnakeBody Portal1, SnakeBody Portal2);
    bool touchPortal();

    void hitwallReact();
    void hitbodyReact();
    
    SnakeBody getHead();
    void senseObstacles(std::vector<SnakeBody> Obstacles);
    bool touchObstacles();

private:
    const int mGameBoardWidth;
    const int mGameBoardHeight;
    // Snake information
    const int mInitialSnakeLength;
    Direction mDirection;
    SnakeBody mFood;
    SnakeBody mPortal1, mPortal2;
    std::vector<SnakeBody> mSnake;
    std::vector<SnakeBody> mObstacles;
};

#endif
