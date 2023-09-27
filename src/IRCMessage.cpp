/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/05 23:29:26 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/26 01:22:54 by hmokhtar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IRCMessage.hpp"
#include "../include/RPL.hpp"
#include <iostream>

std::string  focn(const std::string rawMessage)
{
    // Find the first non-whitespace character from the beginning.
    size_t start = rawMessage.find_first_not_of(" \t\r\n");

    if (start == std::string::npos) {
        // The string is all whitespace.
        return "";
    }

    // Find the last non-whitespace character from the end.
    size_t end = rawMessage.find_last_not_of(" \t\r\n");

    // Extract the trimmed substring.
    return rawMessage.substr(start, end - start + 1);
}

void IRCMessage::parseMsg(const std::string rawMessage)
{
    std::string msg = focn(rawMessage);

    size_t start = 0;
    size_t pos = 0;
    count = 0;

    pos = msg.find(' ');
    if (pos != std::string::npos)
	{
        command = msg.substr(start, pos - start);
        start = pos + 1;
    }
	else
	{
        command = msg;
        return;
    }

    while (start < msg.length())
	{
        pos = msg.find(' ', start);
        if (pos != std::string::npos)
		{
            params.push_back(msg.substr(start, pos - start));
            start = pos + 1;
            count++;
        }
		else
		{
            params.push_back(msg.substr(start, msg.length() - start)); // last param
            count++;
            return;
        }
    }
}

void IRCMessage::authentication(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit)
{
	if (command == "PASS")
		cmd_PASS(server, userit);
	else if (command == "NICK")
		cmd_NICK(client, userit);
	else if (command == "USER")
		cmd_USER(userit);
	else if (userit->getRegistered() == false)
		userit->sendMsg(ERR_NOTREGISTERED(userit->getNick()));
}

void IRCMessage::CmdHandler(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit)
{
	if (command == "JOIN")
		cmd_JOIN(server, userit);
	else if (command == "KICK")
		cmd_KICK(client, server, userit);
	else if (command == "MODE")
		cmd_MODE(client, server, userit);
	else if (command == "TOPIC")
		cmd_TOPIC(server, userit);
	else if (command == "INVITE")
		cmd_INVITE(client, server, userit);
	else if (command == "PRIVMSG")
		cmd_PRIVMSG(client, server, userit);
	else if (command == "PONG")
		cmd_PONG(client, server, userit);
	else if (command == "BOT")
		cmd_BOT(userit);
	else if (command == "QUIT")
		cmd_QUIT(client, server, userit);
	else if (command == "PASS" || command == "NICK" || command == "USER")
		return ;
	else
		userit->sendMsg(ERR_UNKNOWNCOMMAND(userit->getNick(), command));
}
