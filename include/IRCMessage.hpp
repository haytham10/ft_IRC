/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/05 23:29:29 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/09 23:55:03 by hmokhtar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCMESSAGE_HPP
#define IRCMESSAGE_HPP

#include <string>
#include <vector>

class IRCMessage
{
private:
    std::string command;
    std::vector<std::string> params;

    void parseMsg(const std::string &msg);

public:
    IRCMessage(const std::string &msg)
    {
        parseMsg(msg);
    }

    const std::string &getCommand() const
    {
        return command;
    }

    const std::vector<std::string> &getParams() const
    {
        return params;
    }
};

#endif
