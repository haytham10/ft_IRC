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
			else if (client.getUser(getParams()[0]))
				send(userit->getSocket(), "Nickname already in use!\n", 25, 0);
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
						send(userit->getSocket(), "You already set a nickname!\n", 28, 0);
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

			// Make sure getParams()[1] and getParams()[2] have (0, *)
			if (getParams()[1] != "0" || getParams()[2] != "*")
			{
				send(userit->getSocket(), "Invalid USER command parameters!\n", 33, 0);
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
                // Check if the channel is invite-only
                if (channel->isInviteOnlyChannel())
                {
					// TODO: Check if the user is invited to the channel
                    std::string errorMsg = "ERROR :Channel " + channelName + " is invite-only\r\n";
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

									 // Send channel topic if set
                           			 // if (channel->isTopicSetChannel())
                           			 // {
                          			 //     std::string topicMsg = ":" + "GEGA-CHAD IRC " + std::to_string(332) + " " + userit->getNick() + " " + channelName + " :" + channel->getTopic() + "\r\n";
                            		 //     send(userit->getSocket(), topicMsg.c_str(), topicMsg.length(), 0);
                            		 // }
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
                           	// if (channel->isTopicSetChannel())
                           	// {
                          	//     std::string topicMsg = ":" + "GEGA-CHAD IRC " + std::to_string(332) + " " + userit->getNick() + " " + channelName + " :" + channel->getTopic() + "\r\n";
                            //     send(userit->getSocket(), topicMsg.c_str(), topicMsg.length(), 0);
                            // }
						}
						else
						{
							std::string errorMsg = "ERROR :Failed to join channel " + channelName + "\r\n";
							send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
						}
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



// void IRCMessage::cmd_LIST(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit)
// {
//     // Parse the parameters
//     std::vector<std::string> listParams = getParams();

//     // Check if the user provided channel names for filtering
//     if (listParams.size() > 0)
//     {
//         // Handle channel filtering based on the provided channel names
//         std::vector<std::string> channelNames = splitString(listParams[0], ',');

//         for (const std::string &channelName : channelNames)
//         {
//             // Get the channel from the server
//             const IRCChannel* channel = server.getChannel(channelName);

//             if (channel)
//             {
//                 // You can construct and send channel information here
//                 std::string channelInfo = "Channel: " + channel->getName();

//                 // Check if the channel has a topic set
//                 if (channel->isTopicSetChannel())
//                 {
//                     channelInfo += " Topic: " + channel->getTopic();
//                 }

//                 // Add more information as needed (e.g., user count, modes, etc.)

//                 // Send the channel information to the user as an RPL_LIST (322) numeric response
//                 std::string listMsg = ":" + std::string("GEGA-CHAD IRC") + " 322 " + userit->getNick() + " " + channelInfo + "\r\n";
//                 send(userit->getSocket(), listMsg.c_str(), listMsg.length(), 0);
//             }
//             else
//             {
//                 // Channel doesn't exist
//                 std::string errorMsg = "ERROR :Channel " + channelName + " doesn't exist\r\n";
//                 send(userit->getSocket(), errorMsg.c_str(), errorMsg.length(), 0);
//             }
//         }
//     }
//     else
//     {
//         // Handle listing all channels (no channel filtering)
//         std::vector<IRCChannel*> channels = server.getChannels();

//         for (const IRCChannel *channel : channels)
//         {
//             // You can construct and send channel information here
//             std::string channelInfo = "Channel: " + channel->getName();

//             // Check if the channel has a topic set
//             if (channel->isTopicSetChannel())
//             {
//                 channelInfo += " Topic: " + channel->getTopic();
//             }

//             // Add more information as needed (e.g., user count, modes, etc.)

//             // Send the channel information to the user as an RPL_LIST (322) numeric response
//             std::string listMsg = ":" + std::string("GEGA-CHAD IRC 322 ") + userit->getNick() + " " + channelInfo + "\r\n";
//             send(userit->getSocket(), listMsg.c_str(), listMsg.length(), 0);
//         }
//     }

//     // Send an end of list message as an RPL_LISTEND (323) numeric response
//     std::string endOfListMsg = ":" + std::string("GEGA-CHAD IRC 323 ") + userit->getNick() + " :End of LIST\r\n";
//     send(userit->getSocket(), endOfListMsg.c_str(), endOfListMsg.length(), 0);
// }


