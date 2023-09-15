#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../include/IRCMessage.hpp"
#include "../include/IRCServer.hpp"


class IRCUser
{
	private:
		std::string nick;
		std::string username;
		std::string realname;
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

		bool getRegistered() const
		{
			return registered;
		}

		void setRegistered(bool registered)
		{
			this->registered = registered;
		}

		void setNickSet(bool nickSet)
		{
			this->nickSet = nickSet;
		}

		void setUsername(std::string username)
		{
			this->username = username;
		}

		std::string getUsername() const
		{
			return username;
		}

		void setRealname(std::string realname)
		{
			this->realname = realname;
		}

		std::string getRealname() const
		{
			return realname;
		}

};

class IRCClient
{
	private:
		int auth;
		struct pollfd fds[MAX_CLIENTS + 1];
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