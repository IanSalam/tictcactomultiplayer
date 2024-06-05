
#include "tictacto.h"

// Board class implementation
Board::Board() {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            board[i][j] = ' ';
        }
    }
}

void Board::markSymbol(int index, char symbol) {
    int row = index / 3;
    int col = index % 3;
    board[row][col] = symbol;
}

void Board::printBoard() const {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            std::cout << " " << board[i][j] << " ";
            if (j < 2) std::cout << "|";
        }
        std::cout << std::endl;
        if (i < 2) std::cout << "---+---+---" << std::endl;
    }
}

std::string Board::getBoardString() const {
    std::string boardString;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            boardString += " ";
            boardString += board[i][j];
            boardString += " ";
            if (j < 2) boardString += "|";
        }
        boardString += "\n";
        if (i < 2) boardString += "---+---+---\n";
    }
    return boardString;
}

bool Board::checkWin(char symbol) const {
    for (int i = 0; i < 3; ++i) {
        if ((board[i][0] == symbol && board[i][1] == symbol && board[i][2] == symbol) ||
            (board[0][i] == symbol && board[1][i] == symbol && board[2][i] == symbol)) {
            return true;
        }
    }

    if ((board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) ||
        (board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol)) {
        return true;
    }

    return false;
}

bool Board::isCellEmpty(int row, int col) const {
    return board[row][col] == ' ';
}

bool Board::checkDraw() const {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == ' ') {
                return false;
            }
        }
    }
    return true;
}
// Board class implementation
Board::Board() {
    // Initialize the board with cell numbers 1-9
    int count = 1;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            board[i][j] = std::to_string(count)[0]; // Convert int to string and get the first character
            count++;
        }
    }
}

void Board::markSymbol(int number, char symbol) {
    // Convert the cell number to board indices
    int row = (number - 1) / 3;
    int col = (number - 1) % 3;
    board[row][col] = symbol;
}

void Board::printBoard() const {
    std::cout << "-------------" << std::endl;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            std::cout << "| " << board[i][j] << " ";
        }
        std::cout << "|" << std::endl;
        std::cout << "-------------" << std::endl;
    }
}






// Player class implementation
Player::Player(int id, char sym) : ID(id), symbol(sym) {}

char Player::getSymbol() const {
    return symbol;
}

// Game class implementation
Game::Game() : board(), player1(1, 'X'), player2(2, 'O'), statusGame(true) {}

void Game::printResult(const std::string& result) const {
    std::cout << result << std::endl;
}

void Game::gameLoop(SOCKET clientSocket) {
    int currentPlayer = 1;
    while (statusGame) {
        board.printBoard();

        std::string boardString = board.getBoardString();
        if (send(clientSocket, boardString.c_str(), boardString.size(), 0) < 0) {
            std::cerr << "Send failed" << std::endl;
            break;
        }

        int move;
        if (currentPlayer == 1) {
            std::cout << "Player 1, enter a number (0-8): ";
            std::cin >> move;
            if (send(clientSocket, (char*)&move, sizeof(move), 0) < 0) {
                std::cerr << "Send failed" << std::endl;
                break;
            }
        } else {
            if (recv(clientSocket, (char*)&move, sizeof(move), 0) < 0) {
                std::cerr << "Receive failed" << std::endl;
                break;
            }
        }

        if (move < 0 || move > 8) {
            std::cout << "Enter a valid number." << std::endl;
            continue;
        }

        int row = move / 3;
        int col = move % 3;

        if (!board.isCellEmpty(row, col)) {
            std::cout << "Cell already filled, choose another cell." << std::endl;
            continue;
        }

        if (currentPlayer == 1) {
            board.markSymbol(move, player1.getSymbol());
            if (board.checkWin(player1.getSymbol())) {
                boardString = board.getBoardString();
                send(clientSocket, boardString.c_str(), boardString.size(), 0);
                printResult("Player 1 wins!");
                break;
            }
            currentPlayer = 2;
        } else {
            board.markSymbol(move, player2.getSymbol());
            if (board.checkWin(player2.getSymbol())) {
                boardString = board.getBoardString();
                send(clientSocket, boardString.c_str(), boardString.size(), 0);
                printResult("Player 2 wins!");
                break;
            }
            currentPlayer = 1;
        }

        if (board.checkDraw()) {
            boardString = board.getBoardString();
            send(clientSocket, boardString.c_str(), boardString.size(), 0);
            printResult("It's a draw!");
            break;
        }
    }
    closesocket(clientSocket);
}
