/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/05 23:29:29 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/15 04:16:54 by hmokhtar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCMESSAGE_HPP
#define IRCMESSAGE_HPP

#include <string>
#include <vector>
#include "../include/Client.hpp"
#include "../include/IRCServer.hpp"

#define MAX_USERNAME_LENGTH 18

class IRCClient;
class IRCUser;

class IRCMessage
{
	private:
		std::string command;
		std::vector<std::string> params;
		int count;

	public:
		void parseMsg(const std::string msg);
		void authentication(IRCClient &client, IRCServer &server, IRCUser &user);
		////////////// AUTH ///////////////
		void	cmd_PASS(IRCClient &client, IRCServer &server);
		void	cmd_NICK(IRCClient &client, IRCUser &user);
		void	cmd_USER(IRCClient &client, IRCUser &user);
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
			this->params.clear();
			count = 0;
		}

};

#endif
