#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <iostream>
#include <vector>
#include <cstring>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <ctime>
#include "../include/IRCChannels.hpp"
#include <fcntl.h> // Include the fcntl header
#include "../include/Client.hpp"

class IRCChannel;
class IRCUser;

class IRCServer
{
	private:
		int 		port;
		int			serverSocket;
		const char* password;
		std::vector<IRCChannel> channels;

	public:
		void	setup_server();

		IRCServer(int port, const char* password)
		{
			this->port = port;
			this->password = password;
		}

		int getSock() const
		{
			return serverSocket;
		}

		int getPort() const
		{
			return port;
		}

		const char* getPassword() const
		{
			return password;
		}	
		std::vector<IRCChannel>::iterator getChannelIterator(int i)
		{
			return channels.begin() + i;
		}

		// if called it removes the user from the users and admins vector from all channels
		// void removeUserFromChannels(std::vector<IRCUser>::iterator userit)
		// {
		// 	for (std::vector<IRCChannel>::iterator it = channels.begin(); it != channels.end(); ++it)
		// 	{
		// 		it->removeUser(userit);
		// 		if (it->isAdmin(userit))
		// 			it->removeAdmin(userit);
		// 	}
		// }

		// Methods to manage channels
		void 		addChannel(const IRCChannel& channel);
		IRCChannel* getChannel(const std::string& name);
};

#endif