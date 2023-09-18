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
#include "../include/IRCChannels.hpp"
#include <fcntl.h> // Include the fcntl header

class IRCChannel;

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
		// Methods to manage channels
		void 		addChannel(const IRCChannel& channel);
		IRCChannel getChannel(const std::string& name);

};

#endif