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
#include "../include/IRCChannels.hpp"
#include "../include/IRCServer.hpp"

void IRCMessage::cmd_PASS(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit)
{
	(void)client;
	if (userit->getAuth() == 0)
	{
		if (getCount() != 0 && getParams()[0] == server.getPassword())
		{
			userit->setAuth(1);		
		}
		else
			send(userit->getSocket(), "Wrong password!\n", 16, 0);
	}
	else
		send(userit->getSocket(), "You are already authenticated with password!\n", 45, 0);
}

void IRCMessage::cmd_NICK(IRCClient &client, std::vector<IRCUser>::iterator userit)
{
	if (userit->getAuth() == 1)
	{
		std::string str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
		
		if (getCount() != 0)
		{
			if (getParams()[0].find_first_not_of(str) != std::string::npos)
				send(userit->getSocket(), "Invalid nickname!\n", 18, 0);
			else
			{
				if (userit->getNick().empty()) // if nick is empty, set it
				{
					userit->setNick(getParams()[0]);
					userit->setNickSet(true); // set nick to true, so that we can't change it again
					userit->setAuth(2);
				}
				else
				{
					if (userit->getNickSet())
					{
						userit->setNick(getParams()[0]);
						userit->setNickSet(true);
					}
					else
						send(userit->getSocket(), "Nickname already in use!\n", 25, 0);
				}
			}
		}
		else
			send(userit->getSocket(), "No nickname given!\n", 19, 0);
	}
	else if (userit->getAuth() == 2)
		send(userit->getSocket(), "You are already authenticated with nickname!\n", 45, 0);
	else
		send(userit->getSocket(), "Please authenticate with password first!\n", 41, 0);
}

void IRCMessage::cmd_USER(IRCClient &client, std::vector<IRCUser>::iterator userit)
{
	if (userit->getAuth() == 2)
	{
		if (getCount() >= 4)
		{
			// Check if the user already set a nickname
			if (!userit->getNickSet())
			{
				send(userit->getSocket(), "Please set a nickname first!\n", 29, 0);
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
			userit->setUsername(username);
			userit->setRealname(realname);
			userit->setRegistered(true);

			// Set client authentication level to 3
			userit->setAuth(3);
		}
		else
			send(userit->getSocket(), "Not enough parameters for USER command!\n", 40, 0);
	}
	else if (userit->getAuth() == 3)
		send(userit->getSocket(), "You are already authenticated with username!\n", 45, 0);
	else
		send(userit->getSocket(), "Please authenticate with nickname or password first!\n", 53, 0);
}

void IRCMessage::cmd_JOIN(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit)
{
    // Check if the JOIN command has the required parameters
    if (getCount() < 1)
    {
        std::string errorMsg = "ERROR :Not enough parameters for JOIN command\r\n";
        send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    // Parse the channels from the JOIN command parameters
    std::vector<std::string> joinParams = splitString(getParams()[0], ',');
    std::vector<std::string> keys;

    if (getCount() > 1)
    {
        // Parse keys if provided
		if (getParams()[1].find(',') != std::string::npos)
        	keys = splitString(getParams()[1], ',');
    }

    // Iterate through the channels and try to join each one
    for (size_t i = 0; i < joinParams.size(); i++)
    {
        std::string channelParam = joinParams[i];

        // Check if the channel names start with '#', and add '#' if not provided
        std::string channelName;
        if (channelParam[0] != '#')
        {
            channelName = "#" + channelParam;
        }
        else
        {
            channelName = channelParam;
        }

        // Check if the channel exists on the server
        IRCChannel *channel = server.getChannel(channelName);

        if (channel)
        {
            // Check if the user is already in the channel
            if (channel->isUserInChannel(userit))
            {
                std::string errorMsg = "ERROR :You are already in channel " + channelName + "\r\n";
                send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
            }
            else
            {
                // Check if the channel is invite-only
                if (channel->isInviteOnlyChannel())
                {
					// TODO: Check if the user is invited to the channel
                    std::string errorMsg = "ERROR :Channel " + channelName + " is invite-only\r\n";
                    send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
                }
                else
                {
                    // Check if a key is required and provided
                    if (channel->getKey().empty() || (i < keys.size() && keys[i] == channel->getKey()))
                    {
                        // Add the user to the channel
                        if (channel->addUser(userit))
                        {
                            // Send JOIN message to the user
                            std::string joinMsg = ":" + userit->getNick() + " JOIN " + channelName + "\r\n";
                            send(userit->getSocket(), joinMsg.c_str(), joinMsg.length(), 0);

                            // Send channel topic if set
                            // if (channel->isTopicSetChannel())
                            // {
                            //     std::string topicMsg = ":" + "GEGA-CHAD IRC " + std::to_string(332) + " " + userit->getNick() + " " + channelName + " :" + channel->getTopic() + "\r\n";
                            //     send(userit->getSocket(), topicMsg.c_str(), topicMsg.length(), 0);
                            // }

                            // Send list of users in the channel
                            // sendChannelUserList(userit->getSocket(), "GEGA-CHAD IRC", channelName, channel->getUsers());

                            // // Send end of NAMES message
                            // std::string endOfNamesMsg = ":" + "GEGA-CHAD IRC" + " " + std::to_string(366) + " " + userit->getNick() + " " + channelName + " :End of NAMES list\r\n";
                            // send(userit->getSocket(), endOfNamesMsg.c_str(), endOfNamesMsg.length(), 0);
                        }
                        else
                        {
                            // Failed to add user to the channel (e.g., user limit reached)
                            std::string errorMsg = "ERROR :Failed to join channel " + channelName + "\r\n";
                            send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
                        }
                    }
                    else
                    {
                        // Invalid channel key
                        std::string errorMsg = "ERROR :Invalid key for channel " + channelName + "\r\n";
                        send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
                    }
                }
            }
        }
        else
        {
            // Channel doesn't exist, create it
            IRCChannel newChannel(channelName, userit);
            server.addChannel(newChannel);

            // Send JOIN message to the user
            std::string joinMsg = ":" + userit->getNick() + " JOIN " + channelName + "\r\n";
            send(userit->getSocket(), joinMsg.c_str(), joinMsg.length(), 0);

            // // Send end of NAMES message since there are no other users in the new channel
            // std::string endOfNamesMsg = ":" + "GEGA-CHAD IRC" + " " + 366 + " " + userit->getNick() + " " + channelName + " :End of NAMES list\r\n";
            // send(userit->getSocket(), endOfNamesMsg.c_str(), endOfNamesMsg.length(), 0);
        }
    }
}
