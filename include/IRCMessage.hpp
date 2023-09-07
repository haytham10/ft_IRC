/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCMessage.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/05 23:29:29 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/05 23:34:14 by hmokhtar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCMESSAGE_HPP
#define IRCMESSAGE_HPP

#include <string>
#include <vector>

class IRCMessage
{
	
	private:
		std::string rawMessage_;
		std::string prefix_;
		std::string command_;
		std::vector<std::string> parameters_;
		bool valid_;

		// Parse the raw IRC message
		void parseMessage();
		
	public:
		IRCMessage(const std::string& rawMessage);

		// Getters for IRC message components
		std::string getPrefix() const;
		std::string getCommand() const;
		std::vector<std::string> getParameters() const;

		// Check if the message is valid
		bool isValid() const;
};

#endif
