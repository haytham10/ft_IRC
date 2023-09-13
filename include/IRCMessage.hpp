/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/05 23:29:29 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/11 02:53:31 by hmokhtar         ###   ########.fr       */
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
		int count;

	public:
		void parseMsg(const std::string msg);
		void cmd_caller();
		/////////////////////////////
		void	cmd_PASS();
		/////////////////////////////
		const std::string &getCommand() const
		{
			return command;
		}

		int getCount() const
		{
			return count;
		}

		const std::vector<std::string> &getParams() const
		{
			return params;
		}

};

#endif
