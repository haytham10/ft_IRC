/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/05 23:29:29 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/17 10:38:25 by hmokhtar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCMESSAGE_HPP
#define IRCMESSAGE_HPP

#include <string>
#include <vector>
#include <sstream>
#include "../include/Client.hpp"
#include "../include/IRCServer.hpp"

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
		void	cmd_PASS(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit);
		void	cmd_NICK(IRCClient &client, std::vector<IRCUser>::iterator userit);
		void	cmd_USER(IRCClient &client, std::vector<IRCUser>::iterator userit);
		///////////////////////////////////

		////////////// CMD ////////////////
		void	cmd_JOIN(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit);
		void	cmd_KICK(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit);
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
};

#endif
