/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/05 23:29:26 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/08 01:43:41 by hmokhtar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IRCMessage.hpp"

void IRCMessage::parseMsg(const std::string& rawMessage) {
    size_t pos = 0;
    size_t start = 0;

    // Check for a prefix (starts with ':')
    if (rawMessage[start] == ':') {
        pos = rawMessage.find(' ');
        if (pos != std::string::npos) {
            prefix = rawMessage.substr(start + 1, pos - start - 1);
            start = pos + 1;
        }
    }

    // Extract the command
    pos = rawMessage.find(' ', start);
    if (pos != std::string::npos) {
        command = rawMessage.substr(start, pos - start);
        start = pos + 1;
    } else {
        // No parameters
        command = rawMessage.substr(start);
        return;
    }

    // Extract parameters
    while (start < rawMessage.length()) {
        if (rawMessage[start] == ':') {
            // Last parameter
            params.push_back(rawMessage.substr(start + 1));
            break;
        }

        pos = rawMessage.find(' ', start);
        if (pos != std::string::npos) {
            params.push_back(rawMessage.substr(start, pos - start));
            start = pos + 1;
        } else {
            params.push_back(rawMessage.substr(start));
            break;
        }
    }
}