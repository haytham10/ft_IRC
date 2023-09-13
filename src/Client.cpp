#include "../include/IRCServer.hpp"
#include "../include/Client.hpp"

void	IRCClient::setup_client(IRCServer &server)
{

    // Set up pollfd structures for server socket and clients
    struct pollfd fds[MAX_CLIENTS + 1];
    fds[0].fd = server.getSock();
    fds[0].events = POLLIN; // Listen for incoming data
    int numClients = 0;

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
                int clientSocket = accept(server.getSock(), (struct sockaddr*)&clientAddress, &clientAddressLength);
                if (clientSocket != -1)
				{
                    // if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1)
					// {
					// 	perror("fcntl");
					// 	close(clientSocket);
					// 	continue;
					// }

                    // Add the new client to the pollfd array
                    numClients++;
                    fds[numClients].fd = clientSocket;
                    fds[numClients].events = POLLIN;  // Listen for incoming data

                    // You can implement further handling for new clients here
                    // For example, send a welcome message, request authentication, etc.
					char buffer[1024];
					ssize_t bytesRead = recv(fds[numClients].fd, buffer, sizeof(buffer), 0);
					if (bytesRead != -1)
					{
						IRCMessage msg;

						msg.parseMsg(buffer);
						msg.cmd_caller();

					}
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
                char buffer[1024];
                ssize_t bytesRead = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                if (bytesRead > 0)
				{
                    // Process the received data (e.g., parse IRC messages and handle commands)
                    std::string receivedData(buffer, bytesRead);

                    // Implement message parsing and command handling here

                    // Example: Echo the received data back to the client
                    send(fds[i].fd, buffer, bytesRead, 0);
                }
				else if (bytesRead == -1)
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