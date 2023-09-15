///////////////////////////////////////////////////////////////////////////////
////////////////////////// THIS IS A TEST ONLY!!! /////////////////////////////
///////////////////////////////////////////////////////////////////////////////


#include "../include/IRCMessage.hpp"
#include "../include/IRCServer.hpp"
#include "../include/Client.hpp"

int main(int argc, char* argv[])
{
    if (argc != 3 || std::string(argv[2]).empty())
	{
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }

	int port;
	try
	{
		port = std::stoi(argv[1]);
	}
	catch (std::exception &e)
	{
		std::cerr << "Invalid port number!" << std::endl;
		return 1;
	}

	if (port <= 0 || port > 65535)
	{
		std::cerr << "Invalid port number!" << std::endl;
		return 1;
	}
	
	// protect port to be decimal only
	IRCServer server(std::atoi(argv[1]), argv[2]);
	server.setup_server();

	IRCClient client;
	client.setup_client(server);


    // Close the server socket
    close(server.getSock());

    return 0;
}
