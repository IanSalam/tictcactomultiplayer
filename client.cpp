#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "tictacto.h"

#pragma comment(lib, "Ws2_32.lib")

void printBoard(const std::string& boardString) {
    std::cout << boardString << std::endl;
}

int main() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = inet_addr("10.252.135.101"); // Replace with server IP address

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server!" << std::endl;

    while (true) {
        char boardBuffer[256];
        int boardBytes = recv(clientSocket, boardBuffer, sizeof(boardBuffer) - 1, 0);
        if (boardBytes <= 0) {
            std::cerr << "Receive board failed" << std::endl;
            break;
        }
        boardBuffer[boardBytes] = '\0';
        printBoard(boardBuffer);

        int move;
        std::cout << "Player 2, enter a number (0-8): ";
        std::cin >> move;

        if (send(clientSocket, (char*)&move, sizeof(move), 0) < 0) {
            std::cerr << "Send failed" << std::endl;
            break;
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
