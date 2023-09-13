/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/05 23:29:26 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/11 01:54:25 by hmokhtar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IRCMessage.hpp"
#include <iostream>

std::string  focn(const std::string rawMessage)
{
	std::string ret;
	size_t pos = 0;
	
	while (rawMessage[pos] != '\r' && rawMessage[pos] != '\n' && rawMessage[pos] != '\0')
	{
		ret += rawMessage[pos];
		pos++;
	}
	return ret;
	
}

void IRCMessage::parseMsg(const std::string rawMessage)
{
    size_t start = 0;
    size_t pos = 0;
	count = 0;
	std::string msg_7afi = focn(rawMessage);
	pos = msg_7afi.find(' ');
	if (pos != std::string::npos)
	{
		command = msg_7afi.substr(start, pos - start);
		start = pos + 1;
	}
	else
	{
		command = msg_7afi;
		return;
	}
	while (start < msg_7afi.length())
	{
		pos = msg_7afi.find(' ', start);
		if (pos != std::string::npos)
		{
			params.push_back(msg_7afi.substr(start, pos - start));
			start = pos + 1;
			count++;
		}
		else
		{
			params.push_back(msg_7afi.substr(start, msg_7afi.length() - start));
			count++;
			return ;
		}
	}
	return ;
}

void IRCMessage::cmd_caller()
{
	
	if (command == "PASS")
		cmd_PASS();

}