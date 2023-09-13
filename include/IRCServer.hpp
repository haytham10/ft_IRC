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
#include <fcntl.h> // Include the fcntl header

const int MAX_CLIENTS = 20; // Maximum number of clients

class IRCServer
{
	private:
		int port;
		const char* password;
		int	serverSocket;

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
};

#endif