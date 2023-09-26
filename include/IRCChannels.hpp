/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCChannels.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/17 18:34:59 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/26 00:32:46 by hmokhtar         ###   ########.fr       */
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
		std::vector<std::string> invited; // store invited users through invite
		std::time_t	createTime;
		std::string topicSetter;
		int userLimit;
		bool isInviteOnly;
		bool isTopicSet;
		bool isSecure;

	public:
		IRCChannel(const std::string &channelName, std::vector<IRCUser>::iterator creator);

		const std::string& getName() const;
		const std::string& getTopic() const;
		const std::string& getKey() const;
		const std::string getMode() const;
		const std::string& getTopicSetter() const;
		const std::string& getModes() const;
		int getUserLimit() const;
		bool isInviteOnlyChannel() const;
		bool isTopicSetChannel() const;
		bool isSecureChannel() const;

		bool addUser(std::vector<IRCUser>::iterator user);
		bool removeUser(std::vector<IRCUser>::iterator user);
		bool isUserInChannel(std::vector<IRCUser>::iterator user);
		bool isUserAvailable(IRCUser *user);
		bool kickUser(IRCUser *user);

		void setTopic(const std::string &newTopic);
		void setTopicSetter(const std::string &setter);
		void setKey(const std::string &newKey);
		void setUserLimit(int limit);
		void setInviteOnlyChannel(bool isInviteOnly);
		void setTopicSetChannel(bool isTopicSet);
		void setSecureChannel(bool isSecure);
		void sendToChannel(std::string &msg, std::vector<IRCUser>::iterator sender);

		void addAdmin(std::vector<IRCUser>::iterator admin);
		void removeAdmin(std::vector<IRCUser>::iterator admin);
		bool isAdmin(std::vector<IRCUser>::iterator user);
		
		void addAdmine(IRCUser *admin);
		void removeAdmine(IRCUser *admin);
		bool isAdmine(IRCUser *user);

		bool inviteUser(const std::string &name);
		bool isUserInvited(const std::string& userName) const;

		void brodcastMsg(const std::string &msg, std::vector<IRCUser>::iterator sender);

		void setTime();
		time_t getTime() const;

		std::string getChannelUsers();

		std::vector<std::string> splitString(const std::string& input, char delimiter);
};

#endif