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
#include "../include/RPL.hpp"

void IRCMessage::cmd_PASS(IRCServer &server, std::vector<IRCUser>::iterator userit)
{
	if (userit->getAuth() == 0)
	{
		if (getCount() != 0 && getParams()[0] == server.getPassword())
			userit->setAuth(1);		
		else
			userit->sendMsg(ERR_PASSWDMISMATCH(userit->getNick()));
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
				userit->sendMsg(ERR_ERRONEUSNICKNAME(userit->getNick(), getParams()[0]));
			else if (client.getUser(getParams()[0]))
				userit->sendMsg(ERR_NICKNAMEINUSE(userit->getNick(), getParams()[0]));
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
						userit->sendMsg(RPL_NICK(userit->getNick(), getParams()[0]));
						userit->setNick(getParams()[0]);
						userit->setNickSet(true);
					}
					else
						userit->sendMsg(ERR_NICKNAMESET(userit->getNick()));
				}
			}
		}
		else
			userit->sendMsg(ERR_NONICKNAMEGIVEN(userit->getNick()));
	}
	else if (userit->getAuth() == 2)
		userit->sendMsg(ERR_NICKAUTHSET(userit->getNick()));
	else
		userit->sendMsg(ERR_PASSWDFIRST(userit->getNick()));
}

void IRCMessage::cmd_USER(IRCClient &client, std::vector<IRCUser>::iterator userit)
{
	if (userit->getAuth() == 2)
	{
		if (getCount() >= 4)
		{
			if (userit->getUsername().empty())
			{
				// Check if the user already set a nickname
				if (!userit->getNickSet())
				{
					userit->sendMsg(ERR_NONICKNAMEGIVEN(userit->getNick()));
					return;
				}

				// Extract username from the USER command
				std::string username = getParams()[0];

				// Make sure getParams()[1] and getParams()[2] have (0, *)
				if (getParams()[1] != "0" || getParams()[2] != "*")
				{
					userit->sendMsg(ERR_NEEDMOREPARAMS(userit->getNick(), command));
					return;
				}

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
				userit->sendMsg(ERR_ALREADYREGISTERED(userit->getNick()));
		}
		else
			userit->sendMsg(ERR_NEEDMOREPARAMS(userit->getNick(), command));
	}
	else if (userit->getAuth() == 3)
		userit->sendMsg(ERR_ALREADYREGISTERED(userit->getNick()));
	else
		userit->sendMsg(ERR_PASSWDFIRST(userit->getNick()));
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
		// Parse keys if provided
		keys = splitString(getParams()[1], ',');

    // Iterate through the channels and try to join each one
    for (size_t i = 0; i < joinParams.size(); i++)
    {
        std::string channelParam = joinParams[i];

        // Check if the channel names start with '#', and return error if not available
        std::string channelName;
		if (channelParam[0] != '#')
		{
			std::string errorMsg = "ERROR :Invalid channel name: " + channelParam + "\r\n";
			send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
			return;
		}
		else
			channelName = channelParam;

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
				if (channel->isSecureChannel())
				{
					if (keys.size() > 0)
					{
						if (keys[i] == channel->getKey())
						{
							// Add the user to the channel
							if (channel->addUser(userit))
							{
								// Send JOIN message to the user
								std::string joinMsg = ":" + userit->getNick() + " JOIN " + channelName + "\r\n";
								send(userit->getSocket(), joinMsg.c_str(), joinMsg.length(), 0);

								//  Send channel topic if set
                        		 if (channel->getTopic() != "")
                       			 {
                      			     std::string topicMsg = userit->getNick() + " " + channelName + " :" + channel->getTopic() + "\r\n";
                        		     send(userit->getSocket(), topicMsg.c_str(), topicMsg.length(), 0);
                        		 }
							}
							else
							{
								std::string errorMsg = "ERROR :Failed to join channel " + channelName + "\r\n";
								send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
							}
						}
						else
						{
							std::string errorMsg = "ERROR :Wrong key for channel " + channelName + "\r\n";
							send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
						}
					}
					else
					{
						std::string errorMsg = "ERROR :Channel " + channelName + " is password protected\r\n";
						send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
					}
				}
				else
				{
					// Add the user to the channel
					if (channel->addUser(userit))
					{
						// Send JOIN message to the user
						std::string joinMsg = ":" + userit->getNick() + " JOIN " + channelName + "\r\n";
						send(userit->getSocket(), joinMsg.c_str(), joinMsg.length(), 0);

						// Send channel topic if set
                       	if (channel->getTopic() != "")
                      	{
                      	    std::string topicMsg = userit->getNick() + " " + channelName + " :" + channel->getTopic() + "\r\n";
                            send(userit->getSocket(), topicMsg.c_str(), topicMsg.length(), 0);
                        }
					}
					else
					{
						std::string errorMsg = "ERROR :Failed to join channel " + channelName + "\r\n";
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
        }
    }
}

void IRCMessage::cmd_KICK(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit)
{
    try
	{
        // Check if the KICK command has the required parameters
        if (getCount() < 2)
            throw std::runtime_error("Not enough parameters for KICK command");

        // Parse the channels from the KICK command parameters
        std::vector<std::string> kickParams = splitString(getParams()[0], ',');
        std::vector<std::string> users;

        if (getCount() > 1)
		{
            // Parse users if provided
            users = splitString(getParams()[1], ',');
        }

        for (const std::string& channelParam : kickParams) {
            // Check if the channel names start with '#', and add return error if not available
            std::string channelName;
			if (channelParam[0] != '#')
				throw std::runtime_error("Invalid channel name: " + channelParam);
			else
				channelName = channelParam;

            // Get the channel from the server
            IRCChannel* channel = server.getChannel(channelName);

            if (!channel) {
                throw std::runtime_error("Channel doesn't exist");
            }

            // Check if the user is in the channel
            if (!channel->isUserInChannel(userit)) {
                throw std::runtime_error("You are not in the channel");
            }

            // Check if the user is an admin in the channel
            if (!channel->isAdmin(userit)) {
                throw std::runtime_error("You are not an admin in the channel");
            }

            // Iterate through the users and kick each one
            for (const std::string& userName : users)
			{
                // Get the user from the server
                IRCUser* user = client.getUser(userName);

                if (!user)
                    throw std::runtime_error("User doesn't exist: " + userName);

                // Check if the user is in the channel
                if (!channel->isUserAvailable(user))
                    throw std::runtime_error("User is not in the channel: " + userName);

                // Remove the user from the channel
                if (channel->kickUser(user))
				{
					channel->removeAdmin(static_cast<std::vector<IRCUser>::iterator>(user));
                    // Send KICK message to the user
                    std::string kickMsg = ":" + userit->getNick() + " KICK " + channelName + " " + user->getNick() + "\r\n";
                    send(user->getSocket(), kickMsg.c_str(), kickMsg.length(), 0);
                }
				else
                    throw std::runtime_error("Failed to kick user " + user->getNick() + " from channel " + channelName);
            }
        }
    }
	catch (const std::exception& e)
	{
        std::string errorMsg = "ERROR :" + std::string(e.what()) + "\r\n";
        send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
    }
}

void IRCMessage::cmd_MODE(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit)
{
	try
	{
		// Check if the MODE command has the required parameters
		if (getCount() < 1)
			throw std::runtime_error("Not enough parameters for MODE command");

		// Parse the channels from the MODE command parameters
		std::vector<std::string> modeParams = splitString(getParams()[0], ',');
		std::vector<std::string> modes;

		if (getCount() > 1)
		{
			// Parse modes if provided
			modes = splitString(getParams()[1], ',');
		}

		for (const std::string& channelParam : modeParams)
		{
			// Check if the channel names start with '#', and add return error if not available
			std::string channelName;
			if (channelParam[0] != '#')
				throw std::runtime_error("Invalid channel name: " + channelParam);
			else
				channelName = channelParam;

			// Get the channel from the server
			IRCChannel* channel = server.getChannel(channelName);

			if (!channel)
				throw std::runtime_error("Channel doesn't exist");

			// Check if the user is in the channel
			if (!channel->isUserInChannel(userit))
				throw std::runtime_error("You are not in the channel");

			// Check if the user is an admin in the channel
			if (!channel->isAdmin(userit))
				throw std::runtime_error("You are not an admin in the channel");

			// Iterate through the modes and set each one
			for (const std::string& mode : modes)
			{
				if (mode == "+i")
					channel->setInviteOnlyChannel(true);
				else if (mode == "-i")
					channel->setInviteOnlyChannel(false);
				else if (mode == "+t")
					channel->setTopicSetChannel(true);
				else if (mode == "-t")
					channel->setTopicSetChannel(false);
				else if (mode == "+l")
				{
					if (getCount() < 3)
						throw std::runtime_error("Not enough parameters for MODE command");
					else
					{
						int limit = std::stoi(getParams()[2]);
						channel->setUserLimit(limit);
					}
				}
				else if (mode == "-l")
					channel->setUserLimit(0);
				else if (mode == "+k")
				{
					if (getCount() < 3)
						throw std::runtime_error("Not enough parameters for MODE command");
					else
					{
						channel->setSecureChannel(true);
						channel->setKey(getParams()[2]);
					}
				}
				else if (mode == "-k")
					channel->setSecureChannel(false);
				else if (mode == "+o")
				{
					if (getCount() < 3)
						throw std::runtime_error("Not enough parameters for MODE command");
					else
					{
						IRCUser *user = client.getUser(getParams()[2]);

						if (!user)
							throw std::runtime_error("User doesn't exist: " + getParams()[2]);
						else if (channel->isAdmin(static_cast<std::vector<IRCUser>::iterator>(user)) == 1)
							throw std::runtime_error("User is already an Operator: " + getParams()[2]);
						else
							channel->addAdmin(static_cast<std::vector<IRCUser>::iterator>(user));
					}
				}
				else if (mode == "-o")
				{
					if (getCount() < 3)
						throw std::runtime_error("Not enough parameters for MODE command");
					else
					{
						IRCUser *user = client.getUser(getParams()[2]);
						if (!user)
							throw std::runtime_error("User doesn't exist: " + getParams()[2]);
						else if (channel->isAdmin(static_cast<std::vector<IRCUser>::iterator>(user)) == 0)
							throw std::runtime_error("User is not an Operarot: " + getParams()[2]);
						else
							channel->removeAdmin(static_cast<std::vector<IRCUser>::iterator>(user));
					}
				}
				else
					throw std::runtime_error("Invalid mode: " + mode);
			}
		}
	}
	catch (const std::exception& e)
	{
		std::string errorMsg = "ERROR :" + std::string(e.what()) + "\r\n";
		send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
	}
}

void IRCMessage::cmd_TOPIC(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit)
{
    // Check if the TOPIC command has the required parameters
    if (getCount() < 1)
    {
        std::string errorMsg = "ERROR :Not enough parameters for TOPIC command\r\n";
        send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    // Get the channel name from the command parameters
    std::string channelName = getParams()[0];

    // Check if the channel exists on the server
    IRCChannel *channel = server.getChannel(channelName);

    if (channel)
    {
        // Check if the user is on the channel
        if (channel->isUserInChannel(userit))
        {
            // Check if a new topic is provided
            if (getCount() > 1)
            {
                // Check if the user has the necessary privileges to change the topic
                if (channel->isAdmin(userit) || channel->isTopicSetChannel())
                {
                    // Extract the topic text from the message
                    std::string newTopic = getMessageText();

                    // Set the new topic for the channel
                    channel->setTopic(newTopic);

                    // Send RPL_TOPIC numeric reply to the user
                    std::string topicMsg = userit->getNick() + " " + channelName + " :" + newTopic + "\r\n";
                    send(userit->getSocket(), topicMsg.c_str(), topicMsg.length(), 0);
                }
                else
                {
                    // User doesn't have privileges to change the topic
                    std::string errorMsg = "ERROR :You do not have the necessary privileges to change the topic in channel " + channelName + "\r\n";
                    send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
                }
            }
            else
            {
                // No new topic provided, send the current topic as an RPL_TOPIC numeric reply
                std::string currentTopic = channel->getTopic();
                std::string topicMsg = userit->getNick() + " " + channelName + " :" + currentTopic + "\r\n";
                send(userit->getSocket(), topicMsg.c_str(), topicMsg.length(), 0);
            }
        }
        else
        {
            // User is not in the channel, send ERR_NOTONCHANNEL numeric reply
            std::string errorMsg = "ERROR :You are not on channel " + channelName + "\r\n";
            send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
        }
    }
    else
    {
        // Channel doesn't exist, send ERR_NOSUCHCHANNEL numeric reply
        std::string errorMsg = "ERROR :No such channel " + channelName + "\r\n";
        send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
    }
}

void IRCMessage::cmd_INVITE(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit)
{
    // Check if the INVITE command has the required parameters
    if (getCount() < 2)
    {
        std::string errorMsg = "ERROR :Not enough parameters for INVITE command\r\n";
        send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    // Get the user and channel names from the command parameters
    std::string userName = getParams()[0];
    std::string channelName = getParams()[1];

    // Check if the channel exists on the server
    IRCChannel *channel = server.getChannel(channelName);

    if (channel)
    {
        // Check if the user sending the invite is an admin in the channel
        if (channel->isAdmin(userit))
        {
            // Get the user from the server
            IRCUser *userToInvite = client.getUser(userName);

            if (userToInvite)
            {
                // Check if the user is already in the channel
                if (channel->isUserInChannel(static_cast<std::vector<IRCUser>::iterator>(userToInvite)))
                {
                    std::string errorMsg = "ERROR :User " + userName + " is already in channel " + channelName + "\r\n";
                    send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
                }
                else
                {
                    // Add the user to the invited list of the channel
                    channel->inviteUser(userName);

                    // Send RPL_INVITING numeric reply to the user who sent the invite
                    std::string invitingMsg = userit->getNick() + " " + userName + " " + channelName + "\r\n";
                    send(userit->getSocket(), invitingMsg.c_str(), invitingMsg.length(), 0);

                    // Send INVITE message to the invited user
                    std::string inviteMsg = ":" + userit->getNick() + " INVITE " + userName + " " + channelName + "\r\n";
                    send(userToInvite->getSocket(), inviteMsg.c_str(), inviteMsg.length(), 0);
                }
            }
            else
            {
                // User to invite does not exist, send ERR_NOSUCHNICK numeric reply
                std::string errorMsg = "ERROR :No such user " + userName + "\r\n";
                send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
            }
        }
        else
        {
            // User does not have necessary privileges to invite, send ERR_CHANOPRIVSNEEDED numeric reply
            std::string errorMsg = "ERROR :You do not have the necessary privileges to invite to channel " + channelName + "\r\n";
            send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
        }
    }
    else
    {
        // Channel doesn't exist, send ERR_NOSUCHCHANNEL numeric reply
        std::string errorMsg = "ERROR :No such channel " + channelName + "\r\n";
        send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
    }
}

void IRCMessage::cmd_PRIVMSG(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit)
{
    // Check if the PRIVMSG command has the required parameters
    if (getCount() < 2)
    {
        std::string errorMsg = "ERROR :Not enough parameters for PRIVMSG command\r\n";
        send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    // Get the target and message from the command parameters
    std::string target = getParams()[0];
    std::string message = getMessageText();

    // Check if the target is a user or a channel
    if (target[0] == '#')
    {
        // Target is a channel
        IRCChannel *channel = server.getChannel(target);

        if (channel)
        {
            // Check if the user sending the message is in the channel
            if (channel->isUserInChannel(userit))
            {
                // Send the message to all users in the channel
                std::string msgToSend = ":" + userit->getNick() + " PRIVMSG " + target + " :" + message + "\r\n";
                channel->brodcastMsg(msgToSend, userit);
            }
            else
            {
                // User is not in the channel, send ERR_NOTONCHANNEL numeric reply
                std::string errorMsg = "ERROR :You are not on channel " + target + "\r\n";
                send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
            }
        }
        else
        {
            // Channel doesn't exist, send ERR_NOSUCHCHANNEL numeric reply
            std::string errorMsg = "ERROR :No such channel " + target + "\r\n";
            send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
        }
    }
    else
    {
        // Target is a user
        IRCUser *targetUser = client.getUser(target);

        if (targetUser)
        {
            // Send the private message to the target user
            std::string msgToSend = ":" + userit->getNick() + " PRIVMSG " + target + " :" + message + "\r\n";
            send(targetUser->getSocket(), msgToSend.c_str(), msgToSend.length(), 0);
        }
        else
        {
            // Target user doesn't exist, send ERR_NOSUCHNICK numeric reply
            std::string errorMsg = "ERROR :No such user " + target + "\r\n";
            send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
        }
    }
}