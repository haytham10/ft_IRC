/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCCommands.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/11 01:56:05 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/11 02:55:50 by hmokhtar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IRCMessage.hpp"

void IRCMessage::cmd_PASS()
{
	if (getCount() != 0 && getParams()[0] == password)
	{
		send(fds[numClients].fd, "Welcome!\n", 9, 0);
		continue;
	}
	else
		send(fds[numClients].fd, "Wrong password!", 15, 0);
}