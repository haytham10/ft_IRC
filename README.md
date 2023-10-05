# IRC Server in C++

This is a simple IRC (Internet Relay Chat) server implemented in C++. It allows users to connect, communicate, and interact in real-time chat channels. The server is designed to be easy to set up and use, making it a great starting point for learning about network programming and IRC protocol.

## Features

- Basic IRC server functionality.
- Multiple channels support.
- User registration and authentication.
- Real-time communication through sockets.
- Error handling for common network issues.
- Minimalistic design for educational purposes.

## Getting Started

To get started with the IRC server, follow these steps:

1. **Clone the Repository**: Start by cloning this repository to your local machine.

   ```bash
   git clone https://github.com/haytham10/ft_IRC.git
   cd ft_IRC
   ```
   
2. **Compile the Server**: Use your our built-in Makefile to compile

   ```bash
   make ircserv
   ```
   
3. **Run the Server**: Execute the compiled binary to start the server
   ```bash
   ./ircserver <port> <password>
   ```
   Replace <port> with the desired port number (e.g., 6667) and <password> with the server password (optional).

4. **Connect a Client**: Use an IRC client (e.g., LimeChat) to connect to the server. Specify the server address and port, and you can start chatting!

## Usage:
Once the server is running and clients are connected, you can use IRC client software to interact with the server. Here are some basic commands to get you started:

-  `NICK <nickname>`: Set your nickname.
-  `JOIN <channel>`: Join a channel.
-  `PART <channel>`: Leave a channel.
-  `PRIVMSG <target> <message>`: Send a private message to a user or channel.
-  `QUIT [<message>]`: Disconnect from the server (optional quit message).

Refer to the Supported Commands section for a full list of supported IRC commands.

## Supported Commands:

This IRC server supports a subset of common IRC commands. These commands are used for communication, channel management, and user interactions. Below is a list of supported commands:

- `NICK`: Set your nickname.
- `USER`: Set your username.
- `JOIN`: Join a channel.
- `PRIVMSG`: Send a private message to a user or channel.
- `MODE`: Change channel modes.
- `KICK`: Kick users from a channel.
- `TOPIC`: Set or view the channel topic.
- `PONG`: Response to a PING request.
- `QUIT`: Disconnect from the server.

## Contributing:
We welcome contributions to this project. If you'd like to contribute, please follow these steps:

1. Fork the repository.
2. Create a new branch for your feature or bug fix.
3. Make your changes and commit them.
4. Submit a pull request with a clear description of your changes.

Please ensure your code follows the project's coding standards and conventions.

# License
This project is licensed under the MIT License.
