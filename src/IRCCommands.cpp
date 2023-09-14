/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCCommands.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/11 01:56:05 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/14 03:59:37 by hmokhtar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IRCMessage.hpp"

void IRCMessage::cmd_PASS(IRCClient &client, IRCServer &server)
{
	if (getCount() != 0 && getParams()[0] == server.getPassword())
	{
		client.setAuth(1);
	}
	else
		send(client.getFds().fd, "Wrong password!", 15, 0);
}

void IRCMessage::cmd_NICK(IRCClient &client, IRCUser &user)
{
	if (getCount() != 0)
	{
		if (getParams()[0].find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") != std::string::npos)
			send(client.getFds().fd, "Invalid nickname!\n", 18, 0);
		else
		{
			if (user.getNick().empty()) // if nick is empty, set it
			{
				user.setNick(getParams()[0]);
				user.setRegistered(true);
				user.setNickSet(true); // set nick to true, so that we can't change it again
				client.setAuth(2);
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

void IRCMessage::cmd_USER(IRCClient &client, IRCUser &user)
{
	if (getCount() != 0)
	{
		if (user.getNick().empty())
			send(client.getFds().fd, "Please set a nickname first!\n", 29, 0);
		else
		{
			if (user.getNickSet())
			{
				user.setUser(getParams()[0]);
				user.setRegistered(true);
				client.setAuth(3);
			}
			else
				send(client.getFds().fd, "Please set a nickname first!\n", 29, 0);
		}
	}
	else
		send(client.getFds().fd, "No username given!\n", 19, 0);
	
}