#include "../include/IRCServer.hpp"
#include "../include/Client.hpp"

void IRCClient::setup_client(IRCServer &server)
{
    IRCMessage msg;
    IRCUser user;

    // Set up pollfd structures for server socket and clients
    fds[0].fd = server.getSock();
    fds[0].events = POLLIN; // Listen for incoming data
    numClients = 0;
    auth = 0;

    while (true)
    {
        // Use poll to wait for events
        int ready = poll(fds, numClients + 1, -1);
        if (ready == -1)
        {
            perror("poll");
            break;
        }

        // Check for events on the server socket (new connection)
        if (fds[0].revents & POLLIN)
        {
            if (numClients < MAX_CLIENTS)
            {
                // Accept a new client connection
                struct sockaddr_in clientAddress;
                socklen_t clientAddressLength = sizeof(clientAddress);
                int clientSocket = accept(server.getSock(), (struct sockaddr *)&clientAddress, &clientAddressLength);
                if (clientSocket != -1)
                {
                    // Add the new client to the pollfd array
                    numClients++;
                    fds[numClients].fd = clientSocket;
                    fds[numClients].events = POLLIN; // Listen for incoming data
                }
                else
                    perror("accept");
            }
        }

        // Handle events on client sockets
        for (int i = 1; i <= numClients; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                // Handle incoming data from the client (e.g., message parsing and command handling)
                ssize_t bytesRead = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                std::cout << "buffer: " << buffer << std::endl;
                std::cout << "bytesRead: " << bytesRead << std::endl;
                if (bytesRead != -1)
                {
                    msg.parseMsg(buffer);
                    msg.authentication((*this), server, user);
                    if (auth == 3)
                    {
                        fillUsers(user);
                        auth = 0;
                    }

                    // Process the received data (e.g., parse IRC messages and handle commands)
                    std::string receivedData(buffer, bytesRead);

                    // Implement message parsing and command handling here

                    // Example: Echo the received data back to the client
                    send(fds[i].fd, buffer, bytesRead, 0);
                }
                else
                {
                    // Client has disconnected
                    close(fds[i].fd);
                    // Remove the client from the pollfd array
                    for (int j = i; j < numClients; j++)
                    {
                        fds[j] = fds[j + 1];
                    }
                    numClients--;
                }
            }
        }
    }
}
