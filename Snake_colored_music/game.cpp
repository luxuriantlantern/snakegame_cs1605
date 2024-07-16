#include <string>
#include <iostream>
#include <cmath>

// For terminal delay
#include <chrono>
#include <thread>

#include <fstream>
#include <algorithm>

// For music play
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib");

#include "game.h"

//mvwprintw, box
Game::Game()
{
    // Separate the screen to three windows
    this->mWindows.resize(3);
    initscr();
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE); //windows
    init_pair(2, COLOR_GREEN, COLOR_WHITE); //snake, food
    init_pair(3, COLOR_RED, COLOR_WHITE); //obstacle
    init_pair(4, COLOR_BLUE, COLOR_WHITE); //portal
    init_pair(5, COLOR_YELLOW, COLOR_WHITE); //trail
    init_pair(6, COLOR_CYAN, COLOR_WHITE);
    init_pair(7, COLOR_MAGENTA, COLOR_WHITE);
    // If there wasn't any key pressed don't wait for keypress
    nodelay(stdscr, true);
    // Turn on keypad control
    keypad(stdscr, true);
    // No echo for the key pressed
    noecho();
    // No cursor show
    curs_set(0);
    // Get screen and board parameters
    getmaxyx(stdscr, this->mScreenHeight, this->mScreenWidth);
    this->mGameBoardWidth = this->mScreenWidth - this->mInstructionWidth;
    this->mGameBoardHeight = this->mScreenHeight - this->mInformationHeight;

    this->createInformationBoard();
    this->createGameBoard();
    this->createInstructionBoard();

    // Initialize the leader board to be all zeros
    this->mLeaderBoard.assign(this->mNumLeaders, 0);
}

Game::~Game()
{
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        delwin(this->mWindows[i]);
    }
    endwin();
}

void Game::createInformationBoard()
{
    int startY = 0;
    int startX = 0;
    this->mWindows[0] = newwin(this->mInformationHeight, this->mScreenWidth, startY, startX);
}

void Game::renderInformationBoard() const
{
    wattron(mWindows[0], COLOR_PAIR(1));
    mvwprintw(this->mWindows[0], 1, 1, "Welcome to The Snake Game!");

    wattron(mWindows[0], COLOR_PAIR(2));
    mvwprintw(this->mWindows[0], 2, 1, "#");
    wattroff(mWindows[0], COLOR_PAIR(2));
    mvwprintw(this->mWindows[0], 2, 2, " is the symbol of food");

    wattron(mWindows[0], COLOR_PAIR(4));
    mvwprintw(this->mWindows[0], 2, 31, "P");
    wattroff(mWindows[0], COLOR_PAIR(4));
    mvwprintw(this->mWindows[0], 2, 32, " is the symbol of Portal");

    wattron(mWindows[0], COLOR_PAIR(3));
    mvwprintw(this->mWindows[0], 3, 1, "+");
    wattroff(mWindows[0], COLOR_PAIR(3));
    mvwprintw(this->mWindows[0], 3, 2, " is the symbol of obstacle");

    wattron(mWindows[0], COLOR_PAIR(5));
    wattron(this->mWindows[0], A_STANDOUT);
    mvwprintw(this->mWindows[0], 3, 31, " ");
    wattroff(this->mWindows[0], A_STANDOUT);
    wattroff(mWindows[0], COLOR_PAIR(5));
    mvwprintw(this->mWindows[0], 3, 32, " is the symbol of trail (Hard mode only)");

    mvwprintw(this->mWindows[0], 4, 1, "If your snake overlap the trail, you will get an extra life and three points");
    wattroff(mWindows[0], 1);
    wrefresh(this->mWindows[0]);
}

void Game::createGameBoard()
{
    int startY = this->mInformationHeight;
    int startX = 0;
    this->mWindows[1] = newwin(this->mScreenHeight - this->mInformationHeight, this->mScreenWidth - this->mInstructionWidth, startY, startX);
}

void Game::renderGameBoard() const
{
    wrefresh(this->mWindows[1]);
}

void Game::createInstructionBoard()
{
    int startY = this->mInformationHeight;
    int startX = this->mScreenWidth - this->mInstructionWidth;
    this->mWindows[2] = newwin(this->mScreenHeight - this->mInformationHeight, this->mInstructionWidth, startY, startX);
}

void Game::renderInstructionBoard() const
{
    wattron(mWindows[2], COLOR_PAIR(1));
    mvwprintw(this->mWindows[2], 1, 1, "Manual");

    mvwprintw(this->mWindows[2], 3, 1, "Up: W");
    mvwprintw(this->mWindows[2], 4, 1, "Down: S");
    mvwprintw(this->mWindows[2], 5, 1, "Left: A");
    mvwprintw(this->mWindows[2], 6, 1, "Right: D");

    mvwprintw(this->mWindows[2], 8, 1, "Difficulty");
    mvwprintw(this->mWindows[2], 11, 1, "Points");

    mvwprintw(this->mWindows[2], 14, 1, "Life");
    wattroff(mWindows[2], COLOR_PAIR(1));

    wrefresh(this->mWindows[2]);
}


void Game::renderLeaderBoard() const
{
    // If there is not too much space, skip rendering the leader board
    if (this->mScreenHeight - this->mInformationHeight - 14 - 2 < 3 * 2)
    {
        return;
    }
    wattron(mWindows[2], COLOR_PAIR(1));
    mvwprintw(this->mWindows[2], 17, 1, "Leader Board");
    std::string pointString;
    std::string rank;
    for (int i = 0; i < std::min(this->mNumLeaders, this->mScreenHeight - this->mInformationHeight - 17 - 2); i ++)
    {
        pointString = std::to_string(this->mLeaderBoard[i]);
        rank = "#" + std::to_string(i + 1) + ":";
        mvwprintw(this->mWindows[2], 17 + (i + 1), 1, rank.c_str());
        mvwprintw(this->mWindows[2], 17 + (i + 1), 5, pointString.c_str());
    }
    wattroff(mWindows[2], COLOR_PAIR(1));
    wrefresh(this->mWindows[2]);
}

bool Game::renderRestartMenu() const
{
    WINDOW * menu;
    int width = this->mGameBoardWidth * 0.5;
    int height = this->mGameBoardHeight * 0.5;
    int startX = this->mGameBoardWidth * 0.25;
    int startY = this->mGameBoardHeight * 0.25 + this->mInformationHeight;

    menu = newwin(height, width, startY, startX);
    wbkgd(menu, COLOR_PAIR(1));
    box(menu, 0, 0);
    std::vector<std::string> menuItems = {"Restart", "Quit"};

    int index = 0;
    int offset = 4;

    wattron(menu, COLOR_PAIR(1));
    mvwprintw(menu, 1, 1, "Your Final Score:");
    std::string pointString = std::to_string(this->mPoints);
    mvwprintw(menu, 2, 1, pointString.c_str());
    wattron(menu, A_STANDOUT);
    mvwprintw(menu, 0 + offset, 1, menuItems[0].c_str());
    wattroff(menu, A_STANDOUT);
    mvwprintw(menu, 1 + offset, 1, menuItems[1].c_str());

    wrefresh(menu);

    int key;
    while (true)
    {
        key = getch();
        switch(key)
        {
            case 'W':
            case 'w':
            case KEY_UP:
            {
                mciSendString("close moveButton", NULL, 0, NULL);
                mciSendString("open \"music\\moveButton.wav\" type waveaudio alias moveButton", NULL, 0, NULL);
                mciSendString("play moveButton", NULL, 0, NULL);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                index --;
                index = (index < 0) ? menuItems.size() - 1 : index;
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }
            case 'S':
            case 's':
            case KEY_DOWN:
            {
                mciSendString("close moveButton", NULL, 0, NULL);
                mciSendString("open \"music\\moveButton.wav\" type waveaudio alias moveButton", NULL, 0, NULL);
                mciSendString("play moveButton", NULL, 0, NULL);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                index ++;
                index = (index > menuItems.size() - 1) ? 0 : index;
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }
        }
        wattroff(menu, COLOR_PAIR(1));
        wrefresh(menu);
        if (key == ' ' || key == 10)
        {
            mciSendString("close selectButton", NULL, 0, NULL);
            mciSendString("open \"music\\selectButton.wav\" type waveaudio alias selectButton", NULL, 0, NULL);
            mciSendString("play selectButton", NULL, 0, NULL);
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    delwin(menu);

    if (index == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Game::renderStartMenu()
{
    WINDOW * menu;
    int width = this->mGameBoardWidth * 0.5;
    int height = this->mGameBoardHeight * 0.5;
    int startX = this->mGameBoardWidth * 0.25;
    int startY = this->mGameBoardHeight * 0.25 + this->mInformationHeight;

    menu = newwin(height, width, startY, startX);
    wbkgd(menu, COLOR_PAIR(1));
    box(menu, 0, 0);
    std::vector<std::string> menuItems = {"Easy", "Hard"};

    int index = 0;
    int offset = 4;
    wattron(menu, COLOR_PAIR(1));
    mvwprintw(menu, 1, 1, "Choose your game mode:");
    wattron(menu, A_STANDOUT);
    mvwprintw(menu, 0 + offset, 1, menuItems[0].c_str());
    wattroff(menu, A_STANDOUT);
    mvwprintw(menu, 1 + offset, 1, menuItems[1].c_str());

    wrefresh(menu);

    int key;
    while (true)
    {
        key = getch();
        switch(key)
        {
            case 'W':
            case 'w':
            case KEY_UP:
            {
                mciSendString("close moveButton", NULL, 0, NULL);
                mciSendString("open \"music\\moveButton.wav\" type waveaudio alias moveButton", NULL, 0, NULL);
                mciSendString("play moveButton", NULL, 0, NULL);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                index --;
                index = (index < 0) ? menuItems.size() - 1 : index;
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }
            case 'S':
            case 's':
            case KEY_DOWN:
            {
                mciSendString("close moveButton", NULL, 0, NULL);
                mciSendString("open \"music\\moveButton.wav\" type waveaudio alias moveButton", NULL, 0, NULL);
                mciSendString("play moveButton", NULL, 0, NULL);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                index ++;
                index = (index > menuItems.size() - 1) ? 0 : index;
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }
        }
        wattroff(menu, COLOR_PAIR(1));
        wrefresh(menu);
        if (key == ' ' || key == 10)
        {
            mciSendString("close selectButton", NULL, 0, NULL);
            mciSendString("open \"music\\selectButton.wav\" type waveaudio alias selectButton", NULL, 0, NULL);
            mciSendString("play selectButton", NULL, 0, NULL);
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    delwin(menu);

    if (index == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
    
}


void Game::renderPoints() const
{
    std::string pointString = std::to_string(this->mPoints);
    wattron(mWindows[2], COLOR_PAIR(1));
    mvwprintw(this->mWindows[2], 12, 1, pointString.c_str());
    wattroff(mWindows[2], COLOR_PAIR(1));
}

void Game::renderDifficulty() const
{
    std::string difficultyString = std::to_string(this->mDifficulty);
    wattron(mWindows[2], COLOR_PAIR(1));
    mvwprintw(this->mWindows[2], 9, 1, difficultyString.c_str());
    wattroff(mWindows[2], COLOR_PAIR(1));
}

void Game::renderLife() const
{
    std::string lifenumberString = std::to_string(this->mLifeNumber);
    wattron(mWindows[2], COLOR_PAIR(1));
    mvwprintw(this->mWindows[2], 15, 1, "    ");
    mvwprintw(this->mWindows[2], 15, 1, lifenumberString.c_str());
    wattroff(mWindows[2], COLOR_PAIR(1));
}



void Game::initializeGame(bool mode)
{
    // allocate memory for a new snake
	this->mPtrSnake.reset(new Snake(this->mGameBoardWidth, this->mGameBoardHeight, this->mInitialSnakeLength));
    this->mObstacles.clear();
    this->mTrail.clear();
    this->mPortal1 = SnakeBody(0, 0);
    this->mPortal2 = SnakeBody(0, 0);

    this->createRandomFood();
    this->mPtrSnake->senseFood(this->mFood);
    //Create and update the food position.


    this->mPoints = 0;
    this->mDifficulty = 0;
    this->mDifficultyPoints = 0;
    //Set the initialized point and difficulty.

    this->mDelay = 100;
    //Set the initialized delay time.
    this->createRandomPortal();
    this->mPtrSnake->sensePortal(this->mPortal1, this->mPortal2);

    this->createRandomTrail();
    if(mode)
    {
        this->createRandomObstacles();
        this->mPtrSnake->senseObstacles(this->mObstacles);
    }
}

void Game::createRandomFood()//Check all the position which do not overlap with the snake. Choose one randomly.
{
    std::vector <SnakeBody> availgrids;
    for(int i = 1; i < this->mGameBoardWidth - 1; ++ i)
    {
        for(int j = 1; j < this->mGameBoardHeight - 1; ++ j)
        {
            if(this->mPtrSnake->isPartOfSnake(i, j) || this->isPartOfObstacles(SnakeBody(i, j))) continue;
            else availgrids.push_back(SnakeBody(i, j));
        }
    }
    int randidx = std::rand() % availgrids.size();
    this->mFood = availgrids[randidx];
}

bool Game::createRandomPortal()
{
    std::vector <SnakeBody> availgrids;
    for(int i = 1; i < this->mGameBoardWidth - 1; ++ i)
    {
        for(int j = 1; j < this->mGameBoardHeight - 1; ++ j)
        {
            if(this->mPtrSnake->isPartOfSnake(i, j) || this->isPartOfObstacles(SnakeBody(i, j)))continue;
            else availgrids.push_back(SnakeBody(i, j));
        }
    }
    if(availgrids.size() < 100)return false;
    int randidx1 = std::rand() % availgrids.size();
    while(availgrids[randidx1].getX() < 4 || availgrids[randidx1].getY() < 4 ||
          availgrids[randidx1].getX() > this->mGameBoardWidth - 4 ||
          availgrids[randidx1].getY() > this->mGameBoardHeight - 4 ||
          std::abs(availgrids[randidx1].getX() - this->mFood.getX()) < 2 &&
          std::abs(availgrids[randidx1].getY() - this->mFood.getY()) < 2)
        randidx1 = std::rand() % availgrids.size();
    int randidx2 = std::rand() % availgrids.size();
    while(std::abs(availgrids[randidx2].getX() - availgrids[randidx1].getX()) < 4 ||
          std::abs(availgrids[randidx2].getY() - availgrids[randidx1].getY()) < 4 ||
          availgrids[randidx2].getX() < 4 || availgrids[randidx2].getY() < 4 ||
          availgrids[randidx2].getX() > this->mGameBoardWidth - 4 ||
          availgrids[randidx2].getY() > this->mGameBoardHeight - 4 ||
          std::abs(availgrids[randidx1].getX() - this->mFood.getX()) < 2 &&
          std::abs(availgrids[randidx1].getY() - this->mFood.getY()) < 2)
        randidx2 = std::rand() % availgrids.size();
    this->mPortal1 = availgrids[randidx1];
    this->mPortal2 = availgrids[randidx2];
    return true;
}

void Game::createRandomTrail()
{
    this->mTrail.clear();
    std::vector <SnakeBody> availgrids;
    for(int i = 1; i < this->mGameBoardWidth - 1; ++ i)
    {
        for(int j = 1; j < this->mGameBoardHeight - 1; ++ j)
        {
            if(SnakeBody(i, j) == this->mPortal1 || 
               SnakeBody(i, j) == this->mPortal2 ||
               i < 3 || i > this->mGameBoardWidth - 3 ||
               j < 3 || j > this->mGameBoardHeight - 3 ||
               this->isPartOfObstacles(SnakeBody(i, j)))continue;
            else availgrids.push_back(SnakeBody(i, j));
        }
    }
    int randidx = std::rand() % availgrids.size();
    this->mTrail.push_back(availgrids[randidx]);
    int count = 0;
    while(this->mTrail.size() < 2 + this->mTotalLifeNumber * 2&& count <= 100)
    {
        int x = this->mTrail[this->mTrail.size() - 1].getX(), y = this->mTrail[this->mTrail.size() - 1].getY();
        int t = std::rand() % 4;
        switch (t)
        {
            case 0:
                x += 1;
                break;
            case 1:
                x -= 1;
                break;
            case 2:
                y += 1;
                break;
            case 3:
                y -= 1;
                break;
        }
        count ++;
        SnakeBody pt = SnakeBody(x, y);
        if(pt == this->mPortal1 || pt == this->mPortal2 || this->isPartOfTrail(pt) ||
           x < 3 || x > this->mGameBoardWidth - 3 ||
           y < 3 || y > this->mGameBoardHeight - 3)continue;
        this->mTrail.push_back(pt);
    }
}

void Game::createRandomObstacles()
{
    this->mObstacles.clear();
    std::vector <SnakeBody> availgrids;
    for(int i = 2; i < this->mGameBoardWidth - 2; ++ i)
    {
        for(int j = 2; j < this->mGameBoardHeight - 2; ++ j)
        {
            if(SnakeBody(i, j) == this->mPortal1 || 
               SnakeBody(i, j) == this->mPortal2 ||
               std::abs(i - this->mGameBoardWidth / 2) < 3 ||
               std::abs(j - this->mGameBoardHeight / 2) < 3 || 
               this->isPartOfTrail(SnakeBody(i, j)))continue;
            else availgrids.push_back(SnakeBody(i, j));
        }
    }
    for(int i = 1; i <= 6; ++ i)
    {
        int randidx = std::rand() % availgrids.size();
        while(this->isPartOfObstacles(availgrids[randidx]))
        {
            randidx = std::rand() % availgrids.size();
        }
        this->mObstacles.push_back(availgrids[randidx]);
        int count = 0;
        while(this->mObstacles.size() < (i + 2) * (i + 1) / 2 && count <= 100)
        {
            int x = this->mObstacles[this->mObstacles.size() - 1].getX(), y = this->mObstacles[this->mObstacles.size() - 1].getY();
            int t = std::rand() % 4;
            switch (t)
            {
                case 0:
                    x += 1;
                    break;
                case 1:
                    x -= 1;
                    break;
                case 2:
                    y += 1;
                    break;
                case 3:
                    y -= 1;
                    break;
            }
            count ++;
            SnakeBody pt = SnakeBody(x, y);
            if(x <= 2 || y <= 2 || x >= this->mGameBoardWidth - 2 || y >= this->mGameBoardHeight - 2 ||
               std::abs(x - this->mGameBoardWidth / 2) < 3 ||
               std::abs(y - this->mGameBoardHeight / 2) < 3
            )continue;
            if(pt == this->mPortal1 || pt == this->mPortal2 || 
               this->isPartOfObstacles(pt))continue;
            this->mObstacles.push_back(pt);
        }
    }
}

bool Game::isPartOfTrail(SnakeBody point)
{
    int n = this->mTrail.size();
    for(int i = 0; i < n; ++ i)
    {
        if(point == mTrail[i]) return true;
    }
    return false;
}

bool Game::isPartOfObstacles(SnakeBody point)
{
    int n = this->mObstacles.size();
    for(int i = 0; i < n; ++ i)
    {
        if(point == this->mObstacles[i]) return true;
    }
    return false;
}

void Game::renderFood()
{
    wattron(this->mWindows[1], COLOR_PAIR(2));
    if(this->isPartOfTrail(this->mFood))
    {
        // init_pair(1, COLOR_BLUE, COLOR_WHITE);
        wattron(this->mWindows[1], A_STANDOUT);
        mvwaddch(this->mWindows[1], this->mFood.getY(), this->mFood.getX(), this->mFoodSymbol);
        wattroff(this->mWindows[1], A_STANDOUT);
    }
    else
        mvwaddch(this->mWindows[1], this->mFood.getY(), this->mFood.getX(), this->mFoodSymbol);
    wattroff(this->mWindows[1], COLOR_PAIR(2));
}

void Game::renderPortal() const
{
    wattron(this->mWindows[1], COLOR_PAIR(4));
    mvwaddch(this->mWindows[1], this->mPortal1.getY(), this->mPortal1.getX(), this->mPortalSymbol);
    mvwaddch(this->mWindows[1], this->mPortal2.getY(), this->mPortal2.getX(), this->mPortalSymbol);
    wattroff(this->mWindows[1], COLOR_PAIR(4));
}

void Game::renderSnake()
{
    int snakeLength = this->mPtrSnake->getLength();
    std::vector<SnakeBody>& snake = this->mPtrSnake->getSnake();
    // init_pair(1, COLOR_WHITE, COLOR_RED);
    wattron(this->mWindows[1], COLOR_PAIR(2));
    for (int i = 1; i < snakeLength; i ++)
    {
        if(this->isPartOfTrail(snake[i]))
        {
            wattron(this->mWindows[1], A_STANDOUT);
            mvwaddch(this->mWindows[1], snake[i].getY(), snake[i].getX(), this->mSnakeSymbol);
            wattroff(this->mWindows[1], A_STANDOUT);
        }
        else
            mvwaddch(this->mWindows[1], snake[i].getY(), snake[i].getX(), this->mSnakeSymbol);
    }
    if(this->isPartOfTrail(snake[0]))
    {
        wattron(this->mWindows[1], A_STANDOUT);
        mvwaddch(this->mWindows[1], snake[0].getY(), snake[0].getX(), this->mSnakeHeadSymbol);
        wattroff(this->mWindows[1], A_STANDOUT);
    }
    else
        mvwaddch(this->mWindows[1], snake[0].getY(), snake[0].getX(), this->mSnakeHeadSymbol);
    wattroff(this->mWindows[1], COLOR_PAIR(2));
}

void Game::renderTrail()
{
    wattron(this->mWindows[1], COLOR_PAIR(5));
    int n = this->mTrail.size();
    // init_pair(1, COLOR_RED, COLOR_WHITE);
    wattron(this->mWindows[1], A_STANDOUT);
    for(int i = 0; i < n; ++ i)
    {
        int x = mTrail[i].getX(), y = mTrail[i].getY();
        if(this->mPtrSnake->isPartOfSnake(x, y) || mTrail[i] == this->mFood)continue;
        mvwaddch(this->mWindows[1], y, x, this->mTrailSymbol);
    }
    wattroff(this->mWindows[1], A_STANDOUT);
    wattroff(this->mWindows[1], COLOR_PAIR(5));
}

void Game::renderObstacles()
{
    wattron(this->mWindows[1], COLOR_PAIR(3));
    int n = this->mObstacles.size();
    for(int i = 0; i< n; ++ i)
    {
        int x = mObstacles[i].getX(), y = mObstacles[i].getY();
        mvwaddch(this->mWindows[1], y, x, this->mObstacleSymbol);
    }
    wattroff(this->mWindows[1], COLOR_PAIR(3));
}

void Game::controlSnake() const
{
    int key;
    key = getch();
    switch(key)//Give the coordinate direction instruction to the snake.
    {
        case 'W':
        case 'w':
        case KEY_UP:
        {
            this->mPtrSnake->changeDirection(Direction::Up);
            break;
        }
        case 'S':
        case 's':
        case KEY_DOWN:
        {
            this->mPtrSnake->changeDirection(Direction::Down);
            break;
        }
        case 'A':
        case 'a':
        case KEY_LEFT:
        {
            this->mPtrSnake->changeDirection(Direction::Left);
            break;
        }
        case 'D':
        case 'd':
        case KEY_RIGHT:
        {
            this->mPtrSnake->changeDirection(Direction::Right);
            break;
        }
        default:
        {
            break;
        }
    }
}

void Game::renderBoards() const
{
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        werase(this->mWindows[i]);
    }
    this->renderInformationBoard();
    this->renderGameBoard();
    this->renderInstructionBoard();
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        wbkgd(mWindows[i], COLOR_PAIR(1));
        box(this->mWindows[i], 0, 0);
        wrefresh(this->mWindows[i]);
    }
    this->renderLeaderBoard();
}


void Game::adjustDelay()
{
    this->mDifficulty = this->mDifficultyPoints / 5;
    if (this->mDifficultyPoints % 5 == 0)
    {
        this->mDelay = this->mBaseDelay * pow(this->mDifficulty_rate, this->mDifficulty);
    }
}

bool Game::overlapTrail()
{
    for(int i = 0; i < this->mTrail.size(); ++ i)
    {
        if(!this->mPtrSnake->isPartOfSnake(this->mTrail[i].getX(), this->mTrail[i].getY())) return false;
    }
    return true;
}

void Game::runGame(bool mode)
{
    bool moveSuccess;
    int key;
    bool meetPortal = false;
    int meetSize;
    int count = 0;
    int playtime = 0;
    int musictime;
    if (mode) musictime = 160000;
    else musictime = 120000;
    while (true)
    {
        this->controlSnake();
        //Update the screen status
        werase(this->mWindows[1]);
        wbkgd(mWindows[1], COLOR_PAIR(1));
        box(this->mWindows[1], 0, 0);
        
        if (playtime > musictime){
            mciSendString("close bgm", NULL, 0, NULL);
            if (mode){
                mciSendString("open \"music\\bgm_hard.wav\" type waveaudio alias bgm", NULL, 0, NULL);
            } else{
                mciSendString("open \"music\\bgm_easy.wav\" type waveaudio alias bgm", NULL, 0, NULL);
            }
            mciSendString("play bgm", NULL, 0, NULL);
            playtime = 0;
        }

        OBJ meet = this->mPtrSnake->moveFoward();
        
        if(meet == OBJ::wall)
        {
            if(this->mLifeNumber > 0)
            {
                this->mLifeNumber --;
                mciSendString("close hurt", NULL, 0, NULL);
                mciSendString("open \"music\\getHurt.wav\" type waveaudio alias hurt", NULL, 0, NULL);
                mciSendString("play hurt", NULL, 0, NULL);
                this->mPtrSnake->hitwallReact();
            }
            else break;
        }

        if(meet == OBJ::body)
        {
            if(this->mLifeNumber > 0)
            {
                this->mLifeNumber --;
                mciSendString("close hurt", NULL, 0, NULL);
                mciSendString("open \"music\\getHurt.wav\" type waveaudio alias hurt", NULL, 0, NULL);
                mciSendString("play hurt", NULL, 0, NULL);
            }
            else break;
        }

        if(meet == OBJ::obstacle)
        {
            if(this->mLifeNumber > 0)
            {
                this->mLifeNumber --;
                mciSendString("close hurt", NULL, 0, NULL);
                mciSendString("open \"music\\getHurt.wav\" type waveaudio alias hurt", NULL, 0, NULL);
                mciSendString("play hurt", NULL, 0, NULL);
            }
            else break;
        }

        this->renderSnake();

        if(meet == OBJ::food || meet == OBJ::portal && this->mPtrSnake->getHead() == this->mFood)//Create a new food.
        {
            this->mPoints ++;
            mciSendString("close get", NULL, 0, NULL);
            mciSendString("open \"music\\getFood_Life.wav\" type waveaudio alias get", NULL, 0, NULL);
            mciSendString("play get", NULL, 0, NULL);
            this->mDifficultyPoints ++;
            this->createRandomFood();
            this->mPtrSnake->senseFood(this->mFood);
        }
        
        if(this->overlapTrail())
        {
            this->createRandomTrail();
            this->mLifeNumber ++;
            mciSendString("close get", NULL, 0, NULL);
            mciSendString("open \"music\\getFood_Life.wav\" type waveaudio alias get", NULL, 0, NULL);
            mciSendString("play get", NULL, 0, NULL);
            this->mPoints += 3;
            this->mTotalLifeNumber ++;
        }
        this->renderTrail();
        this->renderFood();
        if(count * std::pow(this->mDifficulty_rate, this->mDifficulty) > this->base_Portal_Swift_count)
        {
            if(this->createRandomPortal())
                this->mPtrSnake->sensePortal(this->mPortal1, this->mPortal2);
            else
            {
                this->mPortal1 = SnakeBody(0, 0);
                this->mPortal2 = SnakeBody(this->mGameBoardWidth, this->mGameBoardHeight);
                this->mPtrSnake->sensePortal(this->mPortal1, this->mPortal2);
            }
            count = 0;
        }

        if(meet == OBJ::portal)
            meetPortal = true, meetSize = this->mPtrSnake->getLength(), count = 0;

        if(meetPortal)
        {
            meetSize --;
            if(meetSize == 0)
            {
                meetPortal = false;
                if(this->createRandomPortal())
                    this->mPtrSnake->sensePortal(this->mPortal1, this->mPortal2);
                else
                {
                    this->mPortal1 = SnakeBody(0, 0);
                    this->mPortal2 = SnakeBody(this->mGameBoardWidth, this->mGameBoardHeight);
                    this->mPtrSnake->sensePortal(this->mPortal1, this->mPortal2);
                }
            }
        }
        else
            count++;

        this->renderPortal();
        this->renderObstacles();
        this->renderGameBoard();
        

        this->adjustDelay();
        this->renderDifficulty();
        this->renderPoints();
        this->renderLife();
        wrefresh(this->mWindows[2]);
        playtime = playtime + this->mDelay + 12;
		std::this_thread::sleep_for(std::chrono::milliseconds(this->mDelay));
    }
}

void Game::startGame()
{
    refresh();
    bool choice, mode;
    while (true)
    {
        this->readLeaderBoard();
        this->renderBoards();
        mode = this->renderStartMenu();
        if (mode){
            mciSendString("open \"music\\bgm_hard.wav\" type waveaudio alias bgm", NULL, 0, NULL);
        } else{
            mciSendString("open \"music\\bgm_easy.wav\" type waveaudio alias bgm", NULL, 0, NULL);
        }
        mciSendString("play bgm", NULL, 0, NULL);
        this->initializeGame(mode);
        this->renderFood();
        this->renderPortal();
        this->runGame(mode);
        mciSendString("close bgm", NULL, 0, NULL);
        mciSendString("open \"music\\dead.wav\" type waveaudio alias dead", NULL, 0, NULL);
        mciSendString("play dead", NULL, 0, NULL);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        //mciSendString("close wav", NULL, 0, NULL);
        this->updateLeaderBoard();
        this->writeLeaderBoard();
        choice = this->renderRestartMenu();
        mciSendString("close dead", NULL, 0, NULL);
        if (choice == false)
        {
            break;
        }
    }
}

// https://en.cppreference.com/w/cpp/io/basic_fstream
bool Game::readLeaderBoard()
{
    std::fstream fhand(this->mRecordBoardFilePath, fhand.binary | fhand.in);
    if (!fhand.is_open())
    {
        return false;
    }
    int temp;
    int i = 0;
    while ((!fhand.eof()) && (i < mNumLeaders))
    {
        fhand.read(reinterpret_cast<char*>(&temp), sizeof(temp));
        this->mLeaderBoard[i] = temp;
        i ++;
    }
    fhand.close();
    return true;
}

bool Game::updateLeaderBoard()
{
    bool updated = false;
    int newScore = this->mPoints;
    for (int i = 0; i < this->mNumLeaders; i ++)
    {
        if (this->mLeaderBoard[i] <= this->mPoints)
        {
        int oldScore = this->mLeaderBoard[i];
        this->mLeaderBoard[i] = newScore;
        newScore = oldScore;
        updated = true;
        }
    }
    return updated;
}

bool Game::writeLeaderBoard()
{
    // trunc: clear the data file
    std::fstream fhand(this->mRecordBoardFilePath, fhand.binary | fhand.trunc | fhand.out);
    if (!fhand.is_open())
    {
        return false;
    }
    for (int i = 0; i < this->mNumLeaders; i ++)
    {
        fhand.write(reinterpret_cast<char*>(&this->mLeaderBoard[i]), sizeof(this->mLeaderBoard[i]));;
    }
    fhand.close();
    return true;
}








