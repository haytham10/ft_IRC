/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/05 23:29:26 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/09 23:55:23 by hmokhtar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IRCMessage.hpp"

void IRCMessage::parseMsg(const std::string &rawMessage)
{
    size_t start = 0;
    size_t pos = 0;

    // Extract the command
    pos = rawMessage.find(' ', start);
    if (pos != std::string::npos)
    {
        command = rawMessage.substr(start, pos - start);
        start = pos + 1;
    }
    else
    {
        // No parameters
        command = rawMessage.substr(start);
        return;
    }

    // Extract parameters
    while (start < rawMessage.length())
    {
        pos = rawMessage.find(' ', start);
        if (pos != std::string::npos)
        {
            params.push_back(rawMessage.substr(start, pos - start));
            start = pos + 1;
        }
        else
        {
            params.push_back(rawMessage.substr(start));
            break;
        }
    }
}