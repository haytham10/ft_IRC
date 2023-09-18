/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/05 23:29:26 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/17 10:38:40 by hmokhtar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IRCMessage.hpp"
#include <iostream>

std::string  focn(const std::string rawMessage)
{
	std::string ret;
	size_t pos = 0;
	
	while (rawMessage[pos] != '\r' && rawMessage[pos] != '\n' && rawMessage[pos] != '\0')
	{
		ret += rawMessage[pos];
		pos++;
	}
	return ret;
	
}

void IRCMessage::parseMsg(const std::string rawMessage)
{
    size_t start = 0;
    size_t pos = 0;
	count = 0;
	std::string msg_7afi = focn(rawMessage);
	pos = msg_7afi.find(' ');
	if (pos != std::string::npos)
	{
		command = msg_7afi.substr(start, pos - start);
		start = pos + 1;
	}
	else
	{
		command = msg_7afi;
		return;
	}
	while (start < msg_7afi.length())
	{
		pos = msg_7afi.find(' ', start);
		if (pos != std::string::npos)
		{
			params.push_back(msg_7afi.substr(start, pos - start));
			start = pos + 1;
			count++;
		}
		else
		{
			params.push_back(msg_7afi.substr(start, msg_7afi.length() - start)); // last param
			count++;
			return ;
		}
	}
	return ;
}

void IRCMessage::authentication(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit)
{
	if (command == "PASS")
		cmd_PASS(client, server, userit);
	else if (command == "NICK")
		cmd_NICK(client, userit);
	else if (command == "USER")
		cmd_USER(client, userit);
}

void IRCMessage::CmdHandler(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit)
{
	if (command == "JOIN")
		cmd_JOIN(client, server, userit);
	else if (command == "KICK")
		cmd_KICK(client, server, userit);
	// else if (command == "PRIVMSG")
	// 	cmd_PRIVMSG(client, server, userit);
	// else if (command == "LIST")
	// 	cmd_LIST(client, server, userit);
	// else if (command == "MODE")
	// 	cmd_MODE(client, server, userit);
	// else if (command == "QUIT")
	// 	cmd_QUIT(client, server, userit);
	// else if (command == "TOPIC")
	// 	cmd_TOPIC(client, server, userit);
	// else if (command == "AWAY")
	// 	cmd_AWAY(client, server, userit);
	// else if (command == "INVITE")
	// 	cmd_INVITE(client, server, userit);
	// else if (command == "PING")
	// 	cmd_PING(client, server, userit);
	// else if (command == "PONG")
	// 	cmd_PONG(client, server, userit);
	else
	{
		std::string msg = "ERROR :Unknown command\r\n";
		send(userit->getSocket(), msg.c_str(), msg.length(), 0);
	}
}
