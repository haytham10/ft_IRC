#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "IRCMessage.hpp"
#include "IRCServer.hpp"

const int MAX_CLIENTS = 20; // Maximum number of clients

class IRCUser
{
	private:
		int			fd;
		std::string nick;
		std::string username;
		std::string realname;
		std::string host;
		bool registered;
		bool nickSet;
		int auth;

	public:

		IRCUser()
		{
			auth = 0;
			registered = false;
			nickSet = false;
		}

		int	getAuth() const
		{
			return auth;
		}

		void setAuth(int auth)
		{
			this->auth = auth;
		}

		void setSocket(int fd)
		{
			this->fd = fd;
		}

		int	getSocket() const
		{
			return fd;
		}

		void setHost(std::string host)
		{
			this->host = host;
		}

		std::string getHost() const
		{
			return host;
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

		void sendMsg(const std::string &message)
		{
   	 		send(this->fd, message.c_str(), message.length(), 0);
		}
};

class IRCClient
{
	private:
		int numClients;
		struct pollfd fds[MAX_CLIENTS + 1];
		std::vector<IRCUser> users;

	public:

		void	fillUsers(IRCUser &user)
		{
				users.push_back(user);
		}

		std::vector<IRCUser>::iterator getUsers(int i)
		{
			std::vector<IRCUser>::iterator it = users.begin();
			return (it + i);
		}

		IRCUser *getUser(std::string nickname)
		{
			for (std::vector<IRCUser>::iterator it = users.begin(); it != users.end(); ++it)
			{
				if (it->getNick() == nickname)
					return &(*it);
			}
			return NULL;
		}
		
		void setup_client(IRCServer &server);

		struct pollfd	getFds() const
		{
			return fds[numClients];
		}

		int getFdsNum() const
		{
			return numClients;
		}

		void setNumClients(int numClients)
		{
			this->numClients = numClients;
		}

		int	getNumClients() const
		{
			return numClients;
		}

		void removeUser(std::vector<IRCUser>::iterator userit)
		{
			for (std::vector<IRCUser>::iterator it = users.begin(); it != users.end(); ++it)
			{
				if (it->getNick() == userit->getNick())
				{
					users.erase(it);
					return;
				}
			}
		}
};

#endif