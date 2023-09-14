#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../include/IRCMessage.hpp"
#include "../include/IRCServer.hpp"


class IRCUser
{
	private:
		std::string nick;
		std::string user;
		bool registered;
		bool nickSet;

	public:

		IRCUser()
		{
			registered = false;
			nickSet = false;
		}

		void setNick(std::string nick)
		{
			this->nick = nick;
		}

		std::string getNick() const
		{
			return nick;
		}

		bool getNickSet() const
		{
			return nickSet;
		}

		void setRegistered(bool registered)
		{
			this->registered = registered;
		}

		void setNickSet(bool nickSet)
		{
			this->nickSet = nickSet;
		}

		void setUser(std::string user)
		{
			this->user = user;
		}

};

class IRCClient
{
	private:
		int auth;
		struct pollfd fds[MAX_CLIENTS + 1];
		char buffer[512];
		int numClients;
		std::vector<IRCUser> users;

	public:

		int	getAuth() const
		{
			return auth;
		}

		void setAuth(int auth)
		{
			this->auth = auth;
		}

		void	fillUsers(IRCUser &user)
		{
				users.push_back(user);
		}

		void setup_client(IRCServer &server);

		struct pollfd	getFds() const
		{
			return fds[numClients];
		}

		int	getNumClients() const
		{
			return numClients;
		}
};

#endif