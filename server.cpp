#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

class Board {
private:
    char board[3][3];

public:
    Board() {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                board[i][j] = ' ';
            }
        }
    }

    void markSymbol(int index, char symbol) {
        int row = index / 3;
        int col = index % 3;
        board[row][col] = symbol;
    }

    void printBoard() const {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                std::cout << board[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }

    std::string getBoardString() const {
        std::string boardString;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                boardString += board[i][j];
                boardString += (j == 2) ? '\n' : ' ';
            }
        }
        return boardString;
    }

    bool checkWin(char symbol) const {
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

    bool isCellEmpty(int row, int col) const {
        return board[row][col] == ' ';
    }

    bool checkDraw() const {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (board[i][j] == ' ') {
                    return false;
                }
            }
        }
        return true;
    }
};

class Player {
private:
    int ID;
    char symbol;

public:
    Player(int id, char sym) : ID(id), symbol(sym) {}

    char getSymbol() const {
        return symbol;
    }
};

class Game {
private:
    Board board;
    Player player1;
    Player player2;
    bool statusGame;

public:
    Game() : board(), player1(1, 'X'), player2(2, 'O'), statusGame(true) {}

    void printResult(const std::string& result) const {
        std::cout << result << std::endl;
    }

    void gameLoop(SOCKET clientSocket) {
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
};

int main() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Waiting for connection..." << std::endl;

    SOCKET clientSocket;
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Client connected!" << std::endl;

    Game game;
    game.gameLoop(clientSocket);

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
