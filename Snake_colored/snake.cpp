#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "snake.h"


SnakeBody::SnakeBody()
{
}


SnakeBody::SnakeBody(int x, int y): mX(x), mY(y)
{
}

int SnakeBody::getX() const
{
    return mX;
}

int SnakeBody::getY() const
{
    return mY;
}

bool SnakeBody::operator == (const SnakeBody& snakeBody)
{
    if (this->getX() == snakeBody.getX() && this->getY() == snakeBody.getY())
        return true;
    return false;
}

Snake::Snake(int gameBoardWidth, int gameBoardHeight, int initialSnakeLength): mGameBoardWidth(gameBoardWidth), mGameBoardHeight(gameBoardHeight), mInitialSnakeLength(initialSnakeLength)
{
    this->initializeSnake();
    this->setRandomSeed();
}

void Snake::setRandomSeed()
{
    // use current time as seed for random generator
    std::srand(std::time(nullptr));
}

void Snake::initializeSnake()
{
    // Instead of using a random initialization algorithm
    // We always put the snake at the center of the game mWindows
    int centerX = this->mGameBoardWidth / 2;
    int centerY = this->mGameBoardHeight / 2;

    for (int i = 0; i < this->mInitialSnakeLength; i ++)
    {
        this->mSnake.push_back(SnakeBody(centerX, centerY + i));
    }
    this->mDirection = Direction::Up;
}

bool Snake::isPartOfSnake(int x, int y)//Check (x, y) is or is not a part of the snake, if yes, return true.
{
    for (int i = 0; i < this->getLength(); ++ i)
        if(SnakeBody(x, y) == this->mSnake[i])
            return true;
    return false;
}

/*
 * Assumption:
 * Only the head would hit wall.
 */
bool Snake::hitWall()//If the snake hit the wall, return true.
{
    SnakeBody& hd = this->mSnake[0];
    int x = hd.getX(), y = hd.getY();
    if(x <= 0 || x >= this->mGameBoardWidth - 1 || y <= 0 || y >= this->mGameBoardHeight - 1)
        return true;
    return false;
}

/*
 * The snake head is overlapping with its body
 */
bool Snake::hitSelf()//If the snake hit its body, return true.
{
    SnakeBody& hd = this->mSnake[0];
    for (int i = 1; i < this->mSnake.size(); i ++)
    {
        if (this->mSnake[i] == hd)
        {
            return true;
        }
    }
    return false;
}


bool Snake::touchFood()
{
    SnakeBody newHead = this->createNewHead();
    if (this->mFood == newHead)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Snake::senseFood(SnakeBody food)
{
    this->mFood = food;
}

std::vector<SnakeBody>& Snake::getSnake()
{
    return this->mSnake;
}

bool Snake::changeDirection(Direction newDirection)//The snake can only turn 'left' or 'right' when moving forward
{
    switch (this->mDirection)
    {
        case Direction::Up:
        {
            if(newDirection == Direction::Left || newDirection == Direction::Right)
                this->mDirection = newDirection;
            break;
        }
        case Direction::Down:
        {
            if(newDirection == Direction::Left || newDirection == Direction::Right)
                this->mDirection = newDirection;
            break;
        }
        case Direction::Left:
        {
            if(newDirection == Direction::Up || newDirection == Direction::Down)
                this->mDirection = newDirection;
            break;
        }
        case Direction::Right:
        {
            if(newDirection == Direction::Up || newDirection == Direction::Down)
                this->mDirection = newDirection;
            break;     
        }
    }
    return false;
}


SnakeBody Snake::createNewHead()//Create a new head on the moving direction.
{
    SnakeBody hd = this->mSnake[0];
    int x = hd.getX(), y = hd.getY();
    switch (this->mDirection)
    {
        case Direction::Up:
            y -= 1;
            break;
        case Direction::Down:
            y += 1;
            break;
        case Direction::Left:
            x -= 1;
            break;
        case Direction::Right:
            x += 1;
            break;
    }
    SnakeBody newHead = SnakeBody(x, y);
    return newHead;
}

/*
 * If eat food, return true, otherwise return false
 */
OBJ Snake::moveFoward()
{
    SnakeBody newHead = this->createNewHead();
    if(this->touchFood())//Check if the snake touch the food. If so, add the head only.
    {
        this->mSnake.insert(this->mSnake.begin(), newHead);
        return OBJ::food;
    }
    else if(this->touchPortal())
    {
        int x, y;
        if(newHead == this->mPortal1)
        {
            x = this->mPortal2.getX();
            y = this->mPortal2.getY();
        }
        else
        {
            x = this->mPortal1.getX();
            y = this->mPortal1.getY();
        }
        switch (this->mDirection)
        {
            case Direction::Up:
                y -= 1;
                break;
            case Direction::Down:
                y += 1;
                break;
            case Direction::Left:
                x -= 1;
                break;
            case Direction::Right:
                x += 1;
                break;
        }
        newHead = SnakeBody(x, y);
        this->mSnake.pop_back();
        this->mSnake.insert(this->mSnake.begin(), newHead);
        return OBJ::portal;
    }
    else
    {
        this->mSnake.pop_back();
        this->mSnake.insert(this->mSnake.begin(), newHead);
        if(this->hitSelf())return OBJ::body;
        if(this->hitWall())return OBJ::wall;
        if(this->touchObstacles())return OBJ::obstacle;
        return OBJ::nothing;
    }
}

int Snake::getLength()//Get the length of the snake.
{
    return this->mSnake.size();
}

void Snake::sensePortal(SnakeBody Portal1, SnakeBody Portal2)
{
    this->mPortal1 = Portal1;
    this->mPortal2 = Portal2;
}

bool Snake::touchPortal()
{
    SnakeBody newHead = this->createNewHead();
    if(newHead == this->mPortal1 || newHead == this->mPortal2)
        return true;
    return false;
}

bool Snake::touchObstacles()
{
    SnakeBody newHead = this->createNewHead();
    bool t = false;
    for(int i = 0; i < this->mObstacles.size(); ++ i)
    {
        if(newHead == this->mObstacles[i]) t = true;
    }
    return t;
}

void Snake::hitwallReact()
{
    switch (this->mDirection)
    {
        case Direction::Up :
            this->mSnake[0] = SnakeBody(this->mSnake[0].getX(), this->mGameBoardHeight - 1);
            break;
        case Direction::Down :
            this->mSnake[0] = SnakeBody(this->mSnake[0].getX(), 1);
            break;
        case Direction::Left :
            this->mSnake[0] = SnakeBody(this->mGameBoardWidth - 1, this->mSnake[0].getY());
            break;
        case Direction::Right :
            this->mSnake[0] = SnakeBody(1, this->mSnake[0].getY());
    }
}

SnakeBody Snake::getHead()
{
    return this->mSnake[0];
}

void Snake::senseObstacles(std::vector<SnakeBody> Obstacles)
{
    for(int i = 0; i < Obstacles.size(); ++ i)
    this->mObstacles.push_back(Obstacles[i]);
}