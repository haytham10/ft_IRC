/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCChannels.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/17 18:34:59 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/17 18:34:59 by hmokhtar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCCHANNELS_HPP
#define IRCCHANNELS_HPP

#include "Client.hpp"
#include "IRCMessage.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iterator>

class IRCUser;

class IRCChannel
	{
	private:
		std::string name;
		std::string topic;
		std::string key;
		std::vector<IRCUser> users; // Store pointers to std::vector<IRCUser>::iterator objects
		std::vector<IRCUser> admins; // Store pointers to admin users
		int userLimit;
		bool isPrivate;
		bool isSecret;
		bool isInviteOnly;
		bool isTopicSet;

	public:
		IRCChannel(const std::string &channelName, std::vector<IRCUser>::iterator creator);

		const std::string& getName() const;
		const std::string& getTopic() const;
		const std::string& getKey() const;
		int getUserLimit() const;
		bool isPrivateChannel() const;
		bool isSecretChannel() const;
		bool isInviteOnlyChannel() const;
		bool isTopicSetChannel() const;

		bool addUser(std::vector<IRCUser>::iterator user);
		bool removeUser(std::vector<IRCUser>::iterator user);
		bool isUserInChannel(std::vector<IRCUser>::iterator user);

		void setTopic(const std::string &newTopic);
		void setKey(const std::string &newKey);
		void setUserLimit(int limit);
		void setPrivateChannel(bool isPrivate);
		void setSecretChannel(bool isSecret);
		void setInviteOnlyChannel(bool isInviteOnly);
		void setTopicSetChannel(bool isTopicSet);

		void addAdmin(std::vector<IRCUser>::iterator admin);
		void removeAdmin(std::vector<IRCUser>::iterator admin);
		bool isAdmin(std::vector<IRCUser>::iterator user);

		std::vector<std::string> splitString(const std::string& input, char delimiter);
};

#endif