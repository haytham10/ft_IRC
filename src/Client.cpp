/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 20:11:28 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/27 13:32:17 by hmokhtar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IRCServer.hpp"
#include "../include/Client.hpp"
#include "../include/IRCChannels.hpp"
#include "../include/RPL.hpp"

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
			    IRCUser user;
                struct sockaddr_in clientAddress;
				
                socklen_t clientAddressLength = sizeof(clientAddress);
                int clientSocket = accept(server.getSock(), (struct sockaddr *)&clientAddress, &clientAddressLength);
                user.setSocket(clientSocket);
				user.setHost(inet_ntoa(clientAddress.sin_addr));
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

					std::vector<IRCUser>::iterator userit = getUsers(i - 1);
					
                    // Implement user athentication here...
					msg.authentication((*this), server, userit);	

					if (userit->getAuth() == 3)
					{
						userit->setAuth(69);
						userit->sendMsg(RPL_WELCOME(userit->getNick()));
					}

					// Handle the commands
					if (userit->getRegistered() == true)
						msg.CmdHandler((*this), server, userit);

					// clear message params
					msg.clearParams();
                }
                else
				{
					// Check the error code to determine the cause of recv failure
					if (errno == ECONNRESET || errno == ECONNABORTED)
					{
						// Client has disconnected
						close(fds[i].fd);

						// Remove the client from the pollfd array
						for (int j = i; j < numClients - 1; j++)
							fds[j] = fds[j + 1];

						// Decrement the number of clients
						numClients--;
					}
					else
						perror("recv");
				}
            }
        }
    }
}
