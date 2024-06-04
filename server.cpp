// server.cpp
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

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    if (listen(server_fd, 3) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    SOCKET new_socket;
    sockaddr_in client;
    int clientSize = sizeof(client);
    if ((new_socket = accept(server_fd, (sockaddr*)&client, &clientSize)) == INVALID_SOCKET) {
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    char buffer[1024] = {0};
    int valread = recv(new_socket, buffer, 1024, 0);
    if (valread == SOCKET_ERROR) {
        std::cerr << "Recv failed: " << WSAGetLastError() << std::endl;
        closesocket(new_socket);
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }
    std::cout << "Message from client: " << buffer << std::endl;

    std::string response = "Hello from server";
    send(new_socket, response.c_str(), response.length(), 0);
    std::cout << "Hello message sent" << std::endl;

    closesocket(new_socket);
    closesocket(server_fd);
    WSACleanup();

    return 0;
}
