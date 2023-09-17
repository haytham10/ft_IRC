/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCCommands.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/11 01:56:05 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/17 11:51:04 by hmokhtar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IRCMessage.hpp"

void IRCMessage::cmd_PASS(IRCClient &client, IRCServer &server, IRCUser &user)
{
	(void)client;
	if (user.getAuth() == 0)
	{
		if (getCount() != 0 && getParams()[0] == server.getPassword())
		{
			std::cout << "ANA HNA" << std::endl;
			user.setAuth(1);
			std::cout << "ANA HNA " << user.getAuth() << std::endl;
		}
		else
			send(user.getSocket(), "Wrong password!\n", 16, 0);
	}
	else
		send(user.getSocket(), "You are already authenticated with password!\n", 45, 0);
}

void IRCMessage::cmd_NICK(IRCClient &client, IRCUser &user)
{
	std::cout << "ANA HMAR " << user.getAuth() << std::endl;
	if (user.getAuth() == 1)
	{
		std::string str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
		
		if (getCount() != 0)
		{
			if (getParams()[0].find_first_not_of(str) != std::string::npos)
				send(client.getFds().fd, "Invalid nickname!\n", 18, 0);
			else
			{
				if (user.getNick().empty()) // if nick is empty, set it
				{
					user.setNick(getParams()[0]);
					user.setNickSet(true); // set nick to true, so that we can't change it again
					user.setAuth(2);
				}
				else
				{
					if (user.getNickSet())
					{
						user.setNick(getParams()[0]);
						user.setNickSet(true);
					}
					else
						send(client.getFds().fd, "Nickname already in use!\n", 25, 0);
				}
			}
		}
		else
			send(client.getFds().fd, "No nickname given!\n", 19, 0);
	}
	else if (user.getAuth() == 2)
		send(client.getFds().fd, "You are already authenticated with nickname!\n", 45, 0);
	else
		send(user.getSocket(), "Please authenticate with password first!\n", 41, 0);
}

void IRCMessage::cmd_USER(IRCClient &client, IRCUser &user)
{
	if (user.getAuth() == 2)
	{
		if (getCount() >= 4)
		{
			// Check if the user already set a nickname
			if (!user.getNickSet())
			{
				send(client.getFds().fd, "Please set a nickname first!\n", 29, 0);
				return;
			}

			// Extract username from the USER command
			std::string username = getParams()[0];

			// Extract the real name, which may contain spaces
			std::string realname = "";
			for (size_t i = 3; i < getParams().size(); i++)
			{
				realname += getParams()[i];
				if (i < getParams().size() - 1)
					realname += " "; // Add space between real name words
			}

			// Check and truncate the username if necessary
			if (username.length() > MAX_USERNAME_LENGTH)
				username = username.substr(0, MAX_USERNAME_LENGTH);

			// Update user information
			user.setUsername(username);
			user.setRealname(realname);
			user.setRegistered(true);

			// Set client authentication level to 3
			user.setAuth(3);
		}
		else
			send(client.getFds().fd, "Not enough parameters for USER command!\n", 40, 0);
	}
	else if (user.getAuth() == 3)
		send(client.getFds().fd, "You are already authenticated with username!\n", 45, 0);
	else
		send(client.getFds().fd, "Please authenticate with nickname or password first!\n", 53, 0);
}

