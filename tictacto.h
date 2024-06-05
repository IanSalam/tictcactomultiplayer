#ifndef TICTACTO_H
#define TICTACTO_H

#include <iostream>
#include <string>
#include <winsock2.h> // Include Winsock headers for SOCKET type

// Board class to manage the Tic-Tac-Toe game board
class Board {
private:
    char board[3][3];
public:
    Board();
    void markSymbol(int index, char symbol);
    void printBoard() const;
    std::string getBoardString() const;
    bool checkWin(char symbol) const;
    bool isCellEmpty(int row, int col) const;
    bool checkDraw() const;
};

// Player class to manage player information
class Player {
private:
    int ID;
    char symbol;
public:
    Player(int id, char sym);
    char getSymbol() const;
};

// Game class to manage the game state
class Game {
private:
    Board board;
    Player player1;
    Player player2;
    bool statusGame;
public:
    Game();
    void printResult(const std::string& result) const;
    void gameLoop(SOCKET clientSocket);
};

#endif // TICTACTO_H
