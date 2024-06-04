// client.cpp
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080

int main() {
    WSADATA wsaData;
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    iResult = inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    if (iResult <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    iResult = connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr));
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::string message = "Hello from client";
    send(sock, message.c_str(), message.length(), 0);
    std::cout << "Hello message sent" << std::endl;

    char buffer[1024] = {0};
    int valread = recv(sock, buffer, 1024, 0);
    if (valread == SOCKET_ERROR) {
        std::cerr << "Recv failed: " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    std::cout << "Message from server: " << buffer << std::endl;

    closesocket(sock);
    WSACleanup();

    return 0;
}
