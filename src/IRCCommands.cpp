/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCCommands.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/11 01:56:05 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/27 00:27:52 by hmokhtar         ###   ########.fr       */
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
		userit->sendMsg(ERR_PASSWDSET(userit->getNick()));
}

void IRCMessage::cmd_NICK(IRCClient &client, std::vector<IRCUser>::iterator userit)
{
	if (userit->getAuth() >= 1)
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
					userit->sendMsg(RPL_NICK(userit->getNick(), getParams()[0]));
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

void IRCMessage::cmd_USER(std::vector<IRCUser>::iterator userit)
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

void IRCMessage::cmd_JOIN(IRCServer &server, std::vector<IRCUser>::iterator userit)
{
    // Check if the JOIN command has the required parameters
    if (getCount() < 1)
    {
		userit->sendMsg(ERR_NEEDMOREPARAMS(userit->getNick(), command));
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
			userit->sendMsg(ERR_NOSUCHCHANNEL(userit->getNick(), channelParam));
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
				userit->sendMsg(ERR_ALREADYJOINED(userit->getNick(), channelName));
				return;
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
								userit->sendMsg(RPL_JOIN(userit->getNick(), userit->getUsername(), userit->getHost(), channelName));
								userit->sendMsg(RPL_NAMREPLY(userit->getNick(), channelName, channel->getChannelUsers()));
								userit->sendMsg(RPL_ENDOFNAMES(userit->getNick(), channelName));
								// Brodcast JOIN message to the channel
								channel->brodcastMsg(RPL_JOIN_WATCH(userit->getNick(), userit->getUsername(), channelName), userit);
								channel->brodcastMsg(RPL_NAMREPLY(userit->getNick(), channelName, channel->getChannelUsers()), userit);
								channel->brodcastMsg(RPL_ENDOFNAMES(userit->getNick(), channelName), userit);
								//  Send channel topic if set
                        		 if (channel->getTopic() != "")
									userit->sendMsg(RPL_TOPIC(userit->getNick(), channelName, channel->getTopic()));
							}
							else
								userit->sendMsg(ERR_CANTJOINCHANNEL(userit->getNick(), channelName));
						}
						else
							userit->sendMsg(ERR_BADCHANNELKEY(userit->getNick(), channelName));
					}
					else
						userit->sendMsg(ERR_BADCHANNELKEY(userit->getNick(), channelName));
				}
				else
				{
					// Add the user to the channel
					if (channel->addUser(userit))
					{
						// Send JOIN message to the user
						userit->sendMsg(RPL_JOIN(userit->getNick(), userit->getUsername(), userit->getHost(), channelName));
						userit->sendMsg(RPL_NAMREPLY(userit->getNick(), channelName, channel->getChannelUsers()));
						userit->sendMsg(RPL_ENDOFNAMES(userit->getNick(), channelName));
						// Brodcast JOIN message to the channel
						channel->brodcastMsg(RPL_JOIN_WATCH(userit->getNick(), userit->getUsername(), channelName), userit);
						channel->brodcastMsg(RPL_NAMREPLY(userit->getNick(), channelName, channel->getChannelUsers()), userit);
						channel->brodcastMsg(RPL_ENDOFNAMES(userit->getNick(), channelName), userit);
                       	if (channel->getTopic() != "")
							userit->sendMsg(RPL_TOPIC(userit->getNick(), channelName, channel->getTopic()));
					}
					else
						userit->sendMsg(ERR_CANTJOINCHANNEL(userit->getNick(), channelName));
				}
            }
        }
        else
        {
            // Channel doesn't exist, create it
            IRCChannel newChannel(channelName, userit);
            server.addChannel(newChannel);
			// give channel creator +o mode
			newChannel.setTime();

            // Send JOIN message to the user
			userit->sendMsg(RPL_JOIN(userit->getNick(), userit->getUsername(), userit->getHost(), channelName));
			userit->sendMsg(RPL_NAMREPLY(userit->getNick(), channelName, newChannel.getChannelUsers()));
			userit->sendMsg(RPL_ENDOFNAMES(userit->getNick(), channelName));
        }
    }
}

void IRCMessage::cmd_KICK(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit)
{
    // Check if the KICK command has the required parameters
    if (getCount() < 2)
    {
        userit->sendMsg(ERR_NEEDMOREPARAMS(userit->getNick(), command));
        return;
    }

    // Parse the channels from the KICK command parameters
    std::vector<std::string> kickParams = splitString(getParams()[0], ',');
    std::vector<std::string> users;
    std::string reason = getKickReason();

    if (getCount() > 1)
    {
        // Parse users if provided
        users = splitString(getParams()[1], ',');
    }

    // Iterate over kickParams using a regular for loop
    for (size_t i = 0; i < kickParams.size(); i++)
    {
        const std::string& channelParam = kickParams[i];

        // Check if the channel names start with '#', and add return error if not available
        std::string channelName;
        if (channelParam[0] != '#')
        {
            userit->sendMsg(ERR_NOSUCHCHANNEL(userit->getNick(), channelParam));
            return;
        }
        else
            channelName = channelParam;

        // Get the channel from the server
        IRCChannel* channel = server.getChannel(channelName);

        if (!channel)
        {
            userit->sendMsg(ERR_NOSUCHCHANNEL(userit->getNick(), channelName));
            return;
        }

        // Check if the user is in the channel
        if (!channel->isUserInChannel(userit))
        {
            userit->sendMsg(ERR_NOTONCHANNEL(userit->getNick(), channelName));
            return;
        }

        // Check if the user is an admin in the channel
        if (!channel->isAdmin(userit))
        {
            userit->sendMsg(ERR_CHANOPRIVSNEEDED(userit->getNick(), channelName));
            return;
        }

        // Iterate through the users and kick each one
        for (size_t j = 0; j < users.size(); j++)
        {
            const std::string& userName = users[j];

            // Get the user from the server
            IRCUser* user = client.getUser(userName);

            if (!user)
            {
                userit->sendMsg(ERR_NOSUCHNICK(userit->getNick(), userName));
                continue;  // Continue to the next user
            }

            // Check if the user is in the channel
            if (!channel->isUserAvailable(user))
            {
                userit->sendMsg(ERR_USERNOTINCHANNEL(userit->getNick(), userName, channelName));
                continue;  // Continue to the next user
            }

            // Remove the user from the channel
            if (channel->kickUser(user))
            {
                channel->removeAdmine(user);
                // Send KICK message to the user
                user->sendMsg(RPL_KICK(userit->getNick(), userit->getUsername(), userName, channelName, reason));
            }
            else
                userit->sendMsg(ERR_USERNOTINCHANNEL(userit->getNick(), userName, channelName));
        }
    }
}

void IRCMessage::cmd_MODE(IRCClient& client, IRCServer& server, std::vector<IRCUser>::iterator userit)
{
    // Check if the MODE command has the required parameters
    if (getCount() < 1)
    {
        userit->sendMsg(ERR_NEEDMOREPARAMS(userit->getNick(), command));
        return;
    }

    // Parse the channels from the MODE command parameters
    std::vector<std::string> modeParams = splitString(getParams()[0], ',');
    std::vector<std::string> modes;

	if (getCount() == 1)
	{
		 for (size_t i = 0; i < modeParams.size(); i++)
		{
			const std::string& channelParam = modeParams[i];

			// Check if the channel names start with '#', and add return error if not available
			std::string channelName;
			if (channelParam[0] != '#')
			{
				userit->sendMsg(ERR_NOSUCHCHANNEL(userit->getNick(), channelParam));
				return;
			}
			else
				channelName = channelParam;

			// Get the channel from the server
			IRCChannel* channel = server.getChannel(channelName);

			if (!channel)
			{
				userit->sendMsg(ERR_NOSUCHCHANNEL(userit->getNick(), channelName));
				return;
			}

			// Check if the user is in the channel
			if (!channel->isUserInChannel(userit))
			{
				userit->sendMsg(ERR_NOTONCHANNEL(userit->getNick(), channelName));
				return;
			}

			// Send RPL_CHANNELMODEIS numeric reply to the user
			std::string modeMsg = RPL_CHANNELMODEIS(userit->getNick(), channelName, channel->getMode(), "");
			userit->sendMsg(modeMsg);
			unsigned long time = channel->getTime();
			userit->sendMsg(RPL_CREATIONTIME(userit->getNick(), channelName, std::to_string(time)));
		}
		return;
	}

    if (getCount() > 1)
    {
        // Parse modes if provided
        modes = splitString(getParams()[1], ',');
    }

    for (size_t i = 0; i < modeParams.size(); i++)
    {
		const std::string& channelParam = modeParams[i];

        // Check if the channel names start with '#', and add return error if not available
        std::string channelName;
        if (channelParam[0] != '#')
        {
            userit->sendMsg(ERR_NOSUCHCHANNEL(userit->getNick(), channelParam));
            return;
        }
        else
            channelName = channelParam;

        // Get the channel from the server
        IRCChannel* channel = server.getChannel(channelName);

        if (!channel)
        {
            userit->sendMsg(ERR_NOSUCHCHANNEL(userit->getNick(), channelName));
            return;
        }

        // Check if the user is in the channel
        if (!channel->isUserInChannel(userit))
        {
            userit->sendMsg(ERR_NOTONCHANNEL(userit->getNick(), channelName));
            return;
        }

        // Check if the user is an admin in the channel
        if (!channel->isAdmin(userit))
        {
            userit->sendMsg(ERR_CHANOPRIVSNEEDED(userit->getNick(), channelName));
            return;
        }

        // Iterate through the modes and set each one
        for (size_t j = 0; j < modes.size(); j++)
        {
			const std::string& mode = modes[j];
			
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
                {
                    userit->sendMsg(ERR_NEEDMOREPARAMS(userit->getNick(), command));
                    return;
                }
                else
                {
                    int limit = std::stoi(getParams()[2]);
                    channel->setUserLimit(limit);
                }
            }
            else if (mode == "-l")
                channel->setUserLimit(0);
            else if (mode == "+k" || mode == "-k")
            {
                if (getCount() < 3)
                {
                    userit->sendMsg(ERR_NEEDMOREPARAMS(userit->getNick(), command));
                    return;
                }
                else
                {
                    if (mode == "+k")
                    {
                        channel->setSecureChannel(true);
                        channel->setKey(getParams()[2]);
                    }
                    else
                    {
                        channel->setSecureChannel(false);
                        channel->setKey("");
                    }
                }
            }
            else if (mode == "+o" || mode == "-o")
            {
                if (getCount() < 3)
                {
                    userit->sendMsg(ERR_NEEDMOREPARAMS(userit->getNick(), command));
                    return;
                }
                else
                {
                    IRCUser* targetUser = client.getUser(getParams()[2]);

                    if (!targetUser)
                    {
                        userit->sendMsg(ERR_NOSUCHNICK(userit->getNick(), getParams()[2]));
                        continue;  // Continue to the next mode
                    }

					if (channel->isUserAvailable(targetUser))
					{
						if (mode == "+o")
						{
							if (!channel->isAdmine(targetUser))
								channel->addAdmine(targetUser);
							else
								userit->sendMsg(ERR_CHANOPRIVSNEEDED(userit->getNick(), channelName));
						}
						else if (mode == "-o")
						{
							if (channel->isAdmine(targetUser))
								channel->removeAdmine(targetUser);
							else
								userit->sendMsg(ERR_CHANOPRIVSNEEDED(userit->getNick(), channelName));
						}
					}
					else
						userit->sendMsg(ERR_USERNOTINCHANNEL(userit->getNick(), getParams()[2], channelName));
                }
            }
			else if (mode == "+sn")
				continue;
			else
                userit->sendMsg(ERR_UMODEUNKNOWNFLAG(userit->getNick()));
        }
		
    }
}

void IRCMessage::cmd_TOPIC(IRCServer &server, std::vector<IRCUser>::iterator userit)
{
    // Check if the TOPIC command has the required parameters
    if (getCount() < 1)
    {
		userit->sendMsg(ERR_NEEDMOREPARAMS(userit->getNick(), command));
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
					channel->setTopicSetter(userit->getNick());
                    // Send RPL_TOPIC numeric reply to the user
					std::string topicMsg = RPL_TOPIC(userit->getNick(), channelName, newTopic);
					channel->brodcastMsg(topicMsg, userit);
					userit->sendMsg(topicMsg);
                }
                else
					userit->sendMsg(ERR_CHANOPRIVSNEEDED(userit->getNick(), channelName));
            }
            else
            {
				if (channel->getTopic() == "")
					userit->sendMsg(RPL_NOTOPIC(userit->getNick(), channelName));
				else
				{
					std::time_t topicTime = std::time(NULL);
					userit->sendMsg(RPL_TOPIC(userit->getNick(), channelName, channel->getTopic()));
					userit->sendMsg(RPL_TOPICWHOTIME(userit->getNick(), channelName ,channel->getTopicSetter(), std::to_string(topicTime)));
				}
            }
        }
        else
			userit->sendMsg(ERR_NOTONCHANNEL(userit->getNick(), channelName));
    }
    else
		userit->sendMsg(ERR_NOSUCHCHANNEL(userit->getNick(), channelName));
}

void IRCMessage::cmd_INVITE(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit)
{
    // Check if the INVITE command has the required parameters
    if (getCount() < 2)
    {
		userit->sendMsg(ERR_NEEDMOREPARAMS(userit->getNick(), command));
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
                if (channel->isUserAvailable(userToInvite))
					userit->sendMsg(ERR_USERONCHANNEL(userit->getNick(), userName, channelName));
				else if (channel->isUserInvited(userName))
					userit->sendMsg(ERR_USERONCHANNEL(userit->getNick(), userName, channelName));
                else if (channel->inviteUser(userName))
                {
                    // Send RPL_INVITING numeric reply to the user who sent the invite
					userit->sendMsg(RPL_INVITING(userit->getNick(), userit->getUsername(), userit->getHost(), userName, channelName));

                    // Send INVITE message to the invited user
					userToInvite->sendMsg(RPL_INVITE(userit->getNick(), userName, channelName));
                }
            }
            else
            {
                // User to invite does not exist, send ERR_NOSUCHNICK numeric reply
				userit->sendMsg(ERR_NOSUCHNICK(userit->getNick(), userName));
            }
        }
        else
        {
            // User does not have necessary privileges to invite, send ERR_CHANOPRIVSNEEDED numeric reply
			userit->sendMsg(ERR_CHANOPRIVSNEEDED(userit->getNick(), channelName));
        }
    }
    else
    {
        // Channel doesn't exist, send ERR_NOSUCHCHANNEL numeric reply
		userit->sendMsg(ERR_NOSUCHCHANNEL(userit->getNick(), channelName));
    }
}

void IRCMessage::cmd_PRIVMSG(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit)
{
    // Check if the PRIVMSG command has the required parameters
    if (getCount() < 2)
    {
		userit->sendMsg(ERR_NEEDMOREPARAMS(userit->getNick(), command));
        return;
    }

    // Get the target and message from the command parameters
    std::string target = getParams()[0];
    std::string message = getMessageText();

	if (message.empty())
		userit->sendMsg(ERR_NOTEXTTOSEND(userit->getNick()));

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
                std::string msgToSend = PRIVMSG_TO_CHANNEL(userit->getNick(), userit->getUsername(), target, message);
                channel->brodcastMsg(msgToSend, userit);
            }
            else
            {
                // User is not in the channel, send ERR_NOTONCHANNEL numeric reply
				userit->sendMsg(ERR_NOTONCHANNEL(userit->getNick(), target));
            }
        }
        else
        {
            // Channel doesn't exist, send ERR_NOSUCHCHANNEL numeric reply
			userit->sendMsg(ERR_NOSUCHCHANNEL(userit->getNick(), target));
        }
    }
    else
    {
        // Target is a user
        IRCUser *targetUser = client.getUser(target);

        if (targetUser)
        {
            // Send the private message to the target user
			if (message[0] != (char) ':')
				message = " :" + message;
			targetUser->sendMsg(PRIVMSG_TO_USER(userit->getNick(), userit->getUsername(), userit->getHost(), target, message));
        }
        else
        {
            // Target user doesn't exist, send ERR_NOSUCHNICK numeric reply
			userit->sendMsg(ERR_NORECIPIENT(userit->getNick()));
        }
    }
}

void IRCMessage::cmd_PONG(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit)
{
	(void)client;
	(void)server;
	(void)userit;
	return ;
}

void IRCMessage::cmd_BOT(std::vector<IRCUser>::iterator userit)
{
	// create a commmand response for BOT time to return current time and date,
	if (getCount() < 1)
	{
		userit->sendMsg(ERR_NEEDMOREPARAMS(userit->getNick(), command));
		return;
	}

	std::string command = getParams()[0];
	if (command == "time")
	{
		std::time_t t = std::time(NULL);
		std::string time = std::ctime(&t);
		userit->sendMsg(RPL_TIME(userit->getNick(), time));
	}
	else if (command == "version")
		userit->sendMsg(RPL_VERSION(userit->getNick(), "1.0.0", "UNIX"));
	else if (command == "info")
	{
		std::string info = "Welcome to Our IRC Server!\n\n"
                  "Server Information:\n"
                  "- Server Name: ZAZA Server\n"
                  "- Version: 1.0\n"
                  "- Host: irc.1337.ma\n\n"
                  "Made By:\n"
                  "- Haytham Mokhtari (hmokhtar) aka: AGENT 32\n"
                  "- Hamza El Haddari (hel-hadd)\n"
                  "- Amal Senhaji (amsenhaj)\n\n"
                  "Supported Commands:\n"
                  "- JOIN: Join a channel.\n"
                  "- PART: Leave a channel.\n"
                  "- PRIVMSG: Send private messages.\n"
                  "- MODE: Change channel modes.\n"
                  "- KICK: Kick users from a channel.\n"
                  "- NICK: Change your nickname.\n"
                  "- TOPIC: Set or view the channel topic.\n"
                  "- BOT: Get server information and time.\n\n"
                  "Rules and Guidelines:\n"
                  "- Be respectful to others.\n"
                  "- No spamming or flooding.\n"
                  "- Use common sense and have fun!\n\n"
                  "Thank you for using ZAZA Server!";

		userit->sendMsg(RPL_INFO(userit->getNick(), info));
		userit->sendMsg(RPL_ENDOFINFO(userit->getNick()));
	}
	else
		userit->sendMsg(ERR_UNKNOWNCOMMAND(userit->getNick(), command));
}

void IRCMessage::cmd_QUIT(IRCClient &client, IRCServer &server, std::vector<IRCUser>::iterator userit)
{
	if (getCount() == 1)
	{
		std::string quitMsg = getParams()[0];
		if (quitMsg.empty())
			quitMsg = "Client Quit";
		userit->sendMsg(RPL_QUIT(userit->getNick(), userit->getUsername(), quitMsg));
		
	}
	else
		userit->sendMsg(RPL_QUIT(userit->getNick(), userit->getUsername(), "Client Quit"));
    server.cleanUser(&client, userit);
}