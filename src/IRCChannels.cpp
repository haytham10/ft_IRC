/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCChannels.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/17 18:43:20 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/25 00:59:30 by hmokhtar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IRCChannels.hpp"
#include "../include/RPL.hpp"

IRCChannel::IRCChannel(const std::string &channelName, std::vector<IRCUser>::iterator creator)
	:name(channelName),
	userLimit(0),
	isInviteOnly(false),
	isTopicSet(false),
	isSecure(false)
{
    users.push_back(*creator); // Add the creator as an initial channel member
    admins.push_back(*creator); // Make the creator an admin
}

// Getter methods

std::vector<IRCUser> IRCChannel::getUsers()
{
	return users;
}

const std::string& IRCChannel::getName() const
{
    return name;
}

const std::string& IRCChannel::getTopic() const
{
    return topic;
}

const std::string& IRCChannel::getKey() const
{
    return key;
}

int IRCChannel::getUserLimit() const
{
    return userLimit;
}

bool IRCChannel::isInviteOnlyChannel() const
{
    return isInviteOnly;
}

bool IRCChannel::isTopicSetChannel() const
{
    return isTopicSet;
}

bool IRCChannel::isSecureChannel() const
{
	return isSecure;
}

// Add a user to the channel
bool IRCChannel::addUser(std::vector<IRCUser>::iterator user)
{
    // Check user limit if set
    if (userLimit > 0 && users.size() >= static_cast<size_t>(userLimit))
	{
		user->sendMsg(ERR_CHANNELISFULL(user->getNick(), name));
        return false; // Channel is full
	}

	    // Check if the channel is invite-only and the user is invited
    if (isInviteOnly && !isUserInvited(user->getNick()))
	{
		user->sendMsg(ERR_INVITEONLYCHAN(user->getNick(), name));
        return false; // User is not invited
	}

    // Check if user is already in the channel
    if (!isUserInChannel(user) )
    {
        users.push_back(*user);
        return true; // User added successfully
    }
    return false; // User is already in the channel
}

// Remove a user from the channel
bool IRCChannel::removeUser(std::vector<IRCUser>::iterator user)
{
	for (std::vector<IRCUser>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if (it->getNick() == user->getNick())
		{
			users.erase(it);
			admins.erase(it);
			return true;
		}
	}
	return false;
}

// Check if a user is in the channel
bool IRCChannel::isUserInChannel(std::vector<IRCUser>::iterator user)
{
	for (std::vector<IRCUser>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if (it->getNick() == user->getNick())
			return true;
	}
	return false;
}

// Set the channel topic
void IRCChannel::setTopic(const std::string &newTopic)
{
    topic = newTopic;
    isTopicSet = true;
}

// Set the channel key
void IRCChannel::setKey(const std::string &newKey)
{
    key = newKey;
}

// Set the user limit for the channel
void IRCChannel::setUserLimit(int limit)
{
    userLimit = limit;
}

// Set the channel as invite-only
void IRCChannel::setInviteOnlyChannel(bool isInviteOnly)
{
    this->isInviteOnly = isInviteOnly;
}

// Set the topic as being set or unset
void IRCChannel::setTopicSetChannel(bool isTopicSet)
{
    this->isTopicSet = isTopicSet;
}

// Set the channel as secure
void IRCChannel::setSecureChannel(bool isSecure)
{
	this->isSecure = isSecure;
}

// Add an admin to the channel
void IRCChannel::addAdmin(std::vector<IRCUser>::iterator admin)
{
    if (!isAdmin(admin))
    {
        admins.push_back(*admin);
    }
}

// Remove an admin from the channel
void IRCChannel::removeAdmin(std::vector<IRCUser>::iterator admin)
{
    // Search for the user in the admins vector
	for (std::vector<IRCUser>::iterator it = admins.begin(); it != admins.end(); ++it)
	{
		if (it->getNick() == admin->getNick())
		{
			admins.erase(it);
			return;
		}
	}
}

// Check if a user is an admin in the channel
bool IRCChannel::isAdmin(std::vector<IRCUser>::iterator user)
{
	for (std::vector<IRCUser>::iterator it = admins.begin(); it != admins.end(); ++it)
	{
		if (it->getNick() == user->getNick())
			return true;
	}
	return false;
}

bool IRCChannel::isUserAvailable(IRCUser *user)
{
	for (std::vector<IRCUser>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if (it->getNick() == user->getNick())
			return true;
	}
	return false;
}

bool IRCChannel::kickUser(IRCUser *user)
{
	for (std::vector<IRCUser>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if (it->getNick() == user->getNick())
		{
			users.erase(it);
			return true;
		}
	}
	return false;
}

// Handle invite
bool IRCChannel::inviteUser(const std::string& userName)
{
    // Check if the user is already invited
    if (std::find(invited.begin(), invited.end(), userName) != invited.end())
        return false; // User is already invited

    // Add the user to the invited users list
    invited.push_back(userName);
    return true; // User has been invited
}

// Function to check if a user is invited to the channel
bool IRCChannel::isUserInvited(const std::string& userName) const
{
	return std::find(invited.begin(), invited.end(), userName) != invited.end();
}

void IRCChannel::brodcastMsg(const std::string &msg, std::vector<IRCUser>::iterator sender)
{
	for (std::vector<IRCUser>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if (it->getNick() != sender->getNick())
			it->sendMsg(msg);
	}
}

const std::string IRCChannel::getMode() const
{
	std::string mode = "+";
	
	if (isSecure)
		mode += "s";
	if (isInviteOnly)
		mode += "i";
	if (isTopicSet)
		mode += "t";
	if (userLimit > 0)
		mode += "l";
	if (key != "")
		mode += "k";
	return mode;
}

void IRCChannel::setTime()
{
	createTime = time(0);
}

time_t IRCChannel::getTime() const
{
	return createTime;
}

void IRCChannel::setTopicSetter(const std::string &setter)
{
	topicSetter = setter;
}

const std::string& IRCChannel::getTopicSetter() const
{
	return topicSetter;
}

void IRCChannel::addAdmine(IRCUser *admin)
{
	if (!isAdmine(admin))
	{
		admins.push_back(*admin);
	}
}

void IRCChannel::removeAdmine(IRCUser *admin)
{
	for (std::vector<IRCUser>::iterator it = admins.begin(); it != admins.end(); ++it)
	{
		if (it->getNick() == admin->getNick())
		{
			admins.erase(it);
			return;
		}
	}
}

bool IRCChannel::isAdmine(IRCUser *user)
{
	for (std::vector<IRCUser>::iterator it = admins.begin(); it != admins.end(); ++it)
	{
		if (it->getNick() == user->getNick())
			return true;
	}
	return false;
}

std::string IRCChannel::getChannelUsers()
{
	std::string s_users;
	
	for (std::vector<IRCUser>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if (isAdmin(it))
			s_users += "@"; // Add @ to the nick if the user is an admin
		s_users += it->getNick();
		if (it != users.end() - 1)
			s_users += " ";
	}
	return s_users;
}
