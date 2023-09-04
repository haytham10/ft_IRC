///////////////////////////////////////////////////////////////////////////////
////////////////////////// THIS IS A TEST ONLY!!! /////////////////////////////
///////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <map>
#include <netinet/in.h>
#include <unistd.h>

// Define a structure to represent a connected user
struct IRCUser {
    std::string nickname;
    std::string username;
};

// Define a structure to represent an IRC message
struct IRCMessage {
    std::string prefix;
    std::string command;
    std::vector<std::string> parameters;
};

// Function to parse an IRC message
IRCMessage parseIRCMessage(const std::string& message) {
    IRCMessage ircMessage;
    // Implement your IRC message parsing logic here
    // For simplicity, we'll assume the message format is: "COMMAND param1 param2 ..."
    // You should handle prefix and other details as per the IRC protocol.
    size_t pos = 0;
    size_t spacePos;
    
    // Get the command
    spacePos = message.find(' ');
    ircMessage.command = message.substr(0, spacePos);
    
    // Get parameters
    while (spacePos != std::string::npos) {
        pos = spacePos + 1;
        spacePos = message.find(' ', pos);
        ircMessage.parameters.push_back(message.substr(pos, spacePos - pos));
    }
    
    return ircMessage;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return EXIT_FAILURE;
    }

    int port = atoi(argv[1]);
    const char* password = argv[2];

    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    // Bind the socket to a port
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("bind");
        close(serverSocket);
        return EXIT_FAILURE;
    }

    // Listen for incoming connections
    listen(serverSocket, SOMAXCONN); // SOMAXCONN is the maximum backlog, i.e. the number of connections allowed on the incoming queue

    std::cout << "Server is listening on port " << port << std::endl;

    // Map to store connected users (client sockets mapped to user information)
    std::map<int, IRCUser> connectedUsers;

while (true) {
    // Accept incoming connections
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
    if (clientSocket == -1) {
        perror("accept");
        continue;  // Continue listening for other connections
    }

    // Print a welcome message to the client
    const char* welcomeMessage = "Welcome to the IRC server. Enter IRC commands:\n";
    send(clientSocket, welcomeMessage, strlen(welcomeMessage), 0);

    // Continue receiving and processing data from the client
    while (true) {
        char buffer[1024];
        ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer));
        if (bytesRead <= 0) {
            // Client disconnected or encountered an error
            close(clientSocket);
            break;  // Break from the inner loop and continue listening for other connections
        }

        // Parse the received IRC message
        std::string receivedMessage(buffer, bytesRead);
        IRCMessage ircMessage = parseIRCMessage(receivedMessage);

        // Handle IRC commands here
        // For example, you can handle NICK, USER, PRIVMSG, etc...

		// Handle NICK and USER commands for user registration
		if (ircMessage.command == "NICK" && ircMessage.parameters.size() > 0) {
			// Set the user's nickname
			connectedUsers[clientSocket].nickname = ircMessage.parameters[0];
		} else if (ircMessage.command == "USER" && ircMessage.parameters.size() >= 5) {
			// Set the user's username and real name
			connectedUsers[clientSocket].username = ircMessage.parameters[0];
			std::cout << "User " << connectedUsers[clientSocket].nickname << " registered" << std::endl;

		/// ... Handle other commands here ...

		} else {
			// Unknown command
			const char* unknownCommandMessage = "Unknown command\n";
			send(clientSocket, unknownCommandMessage, strlen(unknownCommandMessage), 0);
		}
		// print data of the added user
		std::cout << "User " << connectedUsers[clientSocket].nickname << " added" << std::endl;
    }
}

    // Close the server socket (not reached in this example)
    close(serverSocket);

    return EXIT_SUCCESS;
}
