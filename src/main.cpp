///////////////////////////////////////////////////////////////////////////////
////////////////////////// THIS IS A TEST ONLY!!! /////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <vector>
#include <cstring>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../include/IRCMessage.hpp"

const int MAX_CLIENTS = 20; // Maximum number of clients

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    const char* password = argv[2];

    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("socket");
        return 1;
    }

    // Set up the server address
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("bind");
        close(serverSocket);
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        perror("listen");
        close(serverSocket);
        return 1;
    }

    std::cout << "Server is listening on port " << port << std::endl;

    // Set up pollfd structures for server socket and clients
    struct pollfd fds[MAX_CLIENTS + 1];
    fds[0].fd = serverSocket;
    fds[0].events = POLLIN; // Listen for incoming data
    int numClients = 0;

    while (true) {
        // Use poll to wait for events
        int ready = poll(fds, numClients + 1, -1);
        if (ready == -1) {
            perror("poll");
            break;
        }

        // Check for events on the server socket (new connection)
        if (fds[0].revents & POLLIN) {
            if (numClients < MAX_CLIENTS) {
                // Accept a new client connection
                struct sockaddr_in clientAddress;
                socklen_t clientAddressLength = sizeof(clientAddress);
                int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
                if (clientSocket != -1) {
                    // Add the new client to the pollfd array
                    numClients++;
                    fds[numClients].fd = clientSocket;
                    fds[numClients].events = POLLIN;
                    std::cout << "New client connected." << std::endl;

                    // Send an authentication request to the client
                    send(clientSocket, "Please enter the password:\r\n", 29, 0);
                }
            }
        }

        // Check for events on client sockets
        for (int i = 1; i <= numClients; i++) {
            if (fds[i].revents & POLLIN) {
                // Handle data from the client
                char buffer[512]; // Adjust buffer size as needed
                int bytesRead = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                if (bytesRead <= 0) {
                    // Handle client disconnect or error
                    close(fds[i].fd);
                    std::cout << "Client disconnected." << std::endl;

                    // Remove the client from the pollfd array
                    fds[i] = fds[numClients];
                    numClients--;
                } else {
                    // Handle received data (e.g., parse IRC messages)
                    // You'll implement message handling here
					parseMessage(buffer);

                    // Check if the received message contains the password
                    if (strstr(buffer, password) != nullptr) {
                        // Password matches, proceed with other commands
                        // Implement your message processing logic here
                    } else {
                        // Password doesn't match, handle authentication failure
                        // You can send an error message to the client and disconnect
                        send(fds[i].fd, "Authentication failed. Disconnecting.\r\n", 38, 0);
                        close(fds[i].fd);
                        std::cout << "Authentication failed. Client disconnected." << std::endl;

                        // Remove the client from the pollfd array
                        fds[i] = fds[numClients];
                        numClients--;
                    }
                }
            }
        }
    }

    // Close the server socket (not reached in this example)
    close(serverSocket);

    return 0;
}
