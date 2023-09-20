#include "../include/IRCServer.hpp"
#include "../include/Client.hpp"
#include "../include/IRCChannels.hpp"

void IRCClient::setup_client(IRCServer &server)
{
    IRCMessage msg;

    // Set up pollfd structures for server socket and clients
    fds[0].fd = server.getSock();
    fds[0].events = POLLIN; // Listen for incoming data
	fds[0].revents = 0;
    numClients = 0;

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
		//	std::cout << "SERVER FD: " << fds[0].fd << std::endl;
            if (numClients < MAX_CLIENTS)
            {
                // Accept a new client connection
                struct sockaddr_in clientAddress;
                socklen_t clientAddressLength = sizeof(clientAddress);
                int clientSocket = accept(server.getSock(), (struct sockaddr *)&clientAddress, &clientAddressLength);
			    IRCUser user;
                user.setSocket(clientSocket);

                //std::cout << "New connection from socket fd" << user.getSocket() << std::endl;
                fillUsers(user);
                if (clientSocket != -1)
                {
                    // Add the new client to the pollfd array
                    numClients++;
                    fds[numClients].fd = clientSocket;
                    fds[numClients].events = POLLIN; // Listen for incoming data
					fds[numClients].revents = 0;
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
				char buffer[512];	
                // Handle incoming data from the client (e.g., message parsing and command handling)
                ssize_t bytesRead = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                if (bytesRead != -1)
                {
                    // Process the received data (e.g., parse IRC messages and handle commands)
                    msg.parseMsg(buffer);

					// clear buffer
					memset(buffer, 0, sizeof(buffer));

                    // Implement user athentication here...
					msg.authentication((*this), server, getUsers(i - 1));	

					if (getUsers(i - 1)->getAuth() == 3)
					{
						send(getUsers(i - 1)->getSocket(), "You are now authenticated!\n", 27, 0);
						getUsers(i - 1)->setAuth(69);
					}
					else if (msg.getCommand() != "PASS" && msg.getCommand() != "NICK" && msg.getCommand() != "USER")
					{
						if (getUsers(i - 1)->getRegistered() == false)
							send(fds[i].fd, "Please authenticate first!\n", 27, 0);
					}

					// Handle the commands
					if (getUsers(i - 1)->getRegistered() == true && getUsers(i - 1)->getAuth() == 69)
						msg.CmdHandler((*this), server, getUsers(i - 1));

					// clear message params
					msg.clearParams();

                    // Example: Echo the received data back to the client
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

