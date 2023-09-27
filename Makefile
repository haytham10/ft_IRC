NAME = ircserv

SRCS = ./src/main.cpp ./src/Client.cpp ./src/IRCServer.cpp ./src/IRCMessage.cpp ./src/IRCCommands.cpp ./src/IRCChannels.cpp

INC = ./include/IRCServer.hpp ./include/IRCMessage.hpp ./include/Client.hpp ./include/IRCChannels.hpp ./include/RPL.hpp

OBJS = $(SRCS:.cpp=.o)

CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98 -g

all: $(NAME)

$(NAME): $(OBJS) $(INC)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "Compilation ....... [OK]"

%.o: %.cpp $(INC)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re