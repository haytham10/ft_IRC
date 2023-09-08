NAME = ircservhayhaymzaml

SRCS = ./src/main.cpp ./src/Client.cpp ./src/IRCConnection.cpp ./src/IRCServer.cpp ./src/IRCMessage.cpp

INC = ./include/IRCConnection.hpp ./include/IRCServer.hpp ./include/IRCMessage.hpp ./include/Client.hpp

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