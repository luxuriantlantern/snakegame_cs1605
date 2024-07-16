#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>
#include <memory>

#include "snake.h"
#include <ncurses.h>

class Game
{
public:
    Game();
    ~Game();

		void createInformationBoard();
    void renderInformationBoard() const;

    void createGameBoard();
    void renderGameBoard() const;

		void createInstructionBoard();
    void renderInstructionBoard() const;

		void loadLeadBoard();
    void updateLeadBoard();
    bool readLeaderBoard();
    bool updateLeaderBoard();
    bool writeLeaderBoard();
    void renderLeaderBoard() const;

		void renderBoards() const;

		void initializeGame(bool mode);
    void runGame(bool mode);
    void renderPoints() const;
    void renderDifficulty() const;

		void createRandomFood();
    void renderFood();
    void renderSnake();
    void controlSnake() const;

		void startGame();
    bool renderRestartMenu() const;
    void adjustDelay();

    void renderPortal() const;
    bool createRandomPortal();

    void renderTrail();
    void createRandomTrail();
    bool isPartOfTrail(SnakeBody point);

    void renderLife() const;
    bool overlapTrail();

    bool renderStartMenu();
    bool isPartOfObstacles(SnakeBody point);
    void createRandomObstacles();
    void renderObstacles();

private:
    // We need to have two windows
    // One is for game introduction
    // One is for game mWindows
    int mScreenWidth;
    int mScreenHeight;
    int mGameBoardWidth;
    int mGameBoardHeight;
    const int mInformationHeight = 6;
    const int mInstructionWidth = 18;
    std::vector<WINDOW *> mWindows;
    // Snake information
    const int mInitialSnakeLength = 2;
    const char mSnakeSymbol = '@';
    const char mSnakeHeadSymbol = 'H';
    std::unique_ptr<Snake> mPtrSnake;

    // Food information
    SnakeBody mFood;
    const char mFoodSymbol = '#';
    int mPoints = 0;
    int mDifficultyPoints = 0;
    int mDifficulty = 0;
    int mBaseDelay = 100;
    int mDelay;
    const float mDifficulty_rate = 0.8;
    const std::string mRecordBoardFilePath = "record.dat";
    std::vector<int> mLeaderBoard;
    const int mNumLeaders = 3;

    const char mPortalSymbol = 'P';
    SnakeBody mPortal1, mPortal2;
    const int base_Portal_Swift_count = 80; // t = base_Portal_Swift_count * mBaseDelay (ms)

    std::vector<SnakeBody> mTrail;
    const char mTrailSymbol = ' ';
    int mLifeNumber = 0;
    int mTotalLifeNumber = 0;

    std::vector<SnakeBody> mObstacles;
    const char mObstacleSymbol = '+';

};

#endif
