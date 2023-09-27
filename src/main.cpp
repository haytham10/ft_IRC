/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/27 13:34:47 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/27 13:38:19 by hmokhtar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IRCMessage.hpp"
#include "../include/IRCServer.hpp"
#include "../include/Client.hpp"
#include <stdlib.h>

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
		port = atoi(argv[1]);
	}
	catch (std::exception &e)
	{
		std::cerr << "Invalid port number!" << std::endl;
		return 1;
	}

	if (port <= 1023 || port > 65535)
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
