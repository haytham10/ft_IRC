#include "../include/IRCServer.hpp"

void	IRCServer::setup_server()
{
	int opt = 1;

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
       perror("socket");
        exit(1);
    }

	// Reuse the socket address
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR,  (char *) &opt, sizeof(opt))){
		perror("setsockopt");
		close(serverSocket);
		exit(1);
	}

	// Make the server socket non-blocking
	if (fcntl(serverSocket, F_SETFL, O_NONBLOCK) == -1) {
		perror("fcntl");
		close(serverSocket);
		exit(1);
	}

    // Set up the server address
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket to the server address
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
		perror("bind");
		close(serverSocket);
		exit(1);
    }

    // Listen for incoming connections
    if (listen(serverSocket, MAX_CLIENTS) == -1) {
    	perror("listen");
    	close(serverSocket);
    	exit(1);
    }

    std::cout << "Server is listening on port " << port << std::endl;

}

// Methods to manage channels
void IRCServer::addChannel(const IRCChannel& channel)
{
	channels.push_back(channel);
}

IRCChannel* IRCServer::getChannel(const std::string& name)
{
	for (std::vector<IRCChannel>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		if (it->getName() == name)
			return &(*it);
	}
	return NULL;
}

void IRCServer::cleanUser(IRCClient* client, std::vector<IRCUser>::iterator userit)
{
    // Remove the user from all channels they are in
    for (std::vector<IRCChannel>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        it->removeUser(userit);
    }

    // Remove the user from the users vector
	client->removeUser(userit);
	
    // Close the user's socket
    close(userit->getSocket());
}
