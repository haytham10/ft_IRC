/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/05 23:29:29 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/26 01:23:00 by hmokhtar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCMESSAGE_HPP
#define IRCMESSAGE_HPP

#include <string>
#include <vector>
#include <sstream>
#include "Client.hpp"
#include "IRCServer.hpp"

#define MAX_USERNAME_LENGTH 18

class IRCClient;
class IRCUser;
class IRCServer;

class IRCMessage
{
	private:
		std::string command;
		std::vector<std::string> params;
		int count;

	public:
		void parseMsg(const std::string msg);
		void authentication(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit);
		void CmdHandler(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit);
		////////////// AUTH ///////////////
		void	cmd_PASS(IRCServer &server, std::vector<IRCUser>::iterator userit);
		void	cmd_NICK(IRCClient &client, std::vector<IRCUser>::iterator userit);
		void	cmd_USER(std::vector<IRCUser>::iterator userit);
		////////////// CMD ////////////////
		void	cmd_JOIN(IRCServer &server, std::vector<IRCUser>::iterator userit);
		void	cmd_KICK(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit);
		void	cmd_MODE(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit);
		void	cmd_LIST(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit);
		void	cmd_TOPIC(IRCServer &server, std::vector<IRCUser>::iterator userit);
		void	cmd_INVITE(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit);
		void	cmd_PRIVMSG(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit);
		void	cmd_PONG(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit);
		void	cmd_BOT(std::vector<IRCUser>::iterator userit);
		///////////////////////////////////
		const std::string &getCommand() const
		{
			return command;
		}

		int getCount() const
		{
			return count;
		}

		const std::vector<std::string> &getParams() const
		{
			return params;
		}

		void	clearParams()
		{
			params.clear();
			count = 0;
		}

		std::vector<std::string> splitString(const std::string& input, char delimiter)
		{
			std::vector<std::string> tokens;
			std::istringstream stream(input);
			std::string token;
			while (std::getline(stream, token, delimiter))
				tokens.push_back(token);
			return tokens;
		}

		std::string getMessageText() const
		{
			std::string msg;
			for (int i = 1; i < count; i++)
			{
				msg += params[i];
				if (i != count - 1)
					msg += " ";
			}
			return msg;
		}

		std::string getKickReason() const
		{
			std::string msg;
			for (int i = 2; i < count; i++)
			{
				msg += params[i];
				if (i != count - 1)
					msg += " ";
			}
			return msg;
		}		
};

#endif
