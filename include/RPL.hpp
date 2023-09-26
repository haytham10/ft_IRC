/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPL.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 19:44:22 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/26 01:53:33 by hmokhtar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RPL_HPP
#define RPL_HPP

#include "Client.hpp"
#include "IRCMessage.hpp"
#include "IRCServer.hpp"
#include "IRCChannels.hpp"

// GENERAL
# define RPL_WELCOME(nickname) (":irc.1337.ma 001 " + nickname + " :Welcome to `ZAZA Server` !" + "\r\n")
# define ERR_UNKNOWNCOMMAND(client, command) (":irc.1337.ma 421 " + client + " " + command + " :Unknown command\r\n")
# define ERR_NOTREGISTERED(client) (":irc.1337.ma 451 " + client + ": You have not registered." + "\r\n")
# define ERR_NEEDMOREPARAMS(client, command) (":irc.1337.ma 461 " + client + " " + command + " :Not enough parameters.\r\n")
# define ERR_NOSUCHCHANNEL(client, channel) (":irc.1337.ma 403 " + client + channel + " :No such channel\r\n")
# define ERR_NOTONCHANNEL(client, channel) (":irc.1337.ma 442 " + client + channel + " :The user is not on this channel.\r\n")
# define ERR_USERNOTINCHANNEL(client, nickname, channel) (":irc.1337.ma 441 " + client + " " + nickname + channel + " :They aren't on that channel\r\n")
# define RPL_NAMREPLY(client, channel, users) (":irc.1337.ma 353 " + client + " = " + channel + " :" + users + "\r\n")
# define RPL_ENDOFNAMES(nickname, channel) (":irc.1337.ma 366 " + nickname + " " + channel + " ::End of /NAMES list." + "\r\n")


// INVITE
# define ERR_USERONCHANNEL(client, nick, channel) (":irc.1337.ma 443 " + client + " " + nick + channel + " :Is already on channel\r\n")
# define RPL_INVITING(nick , user, clientHost, targetClient, channel)  (":irc.1337.ma 341 " + nick + "!" + user + "@" + clientHost + " " + targetClient + " " + channel + "\r\n")
# define RPL_INVITE(client, invited, channel) (":" + client + " INVITE " + invited + " to " + channel + "\r\n")

// JOIN
# define RPL_JOIN(nick, user, clientHost, channel) (":" + nick + "!" + user + "@" +  clientHost  + " JOIN " + channel + "\r\n")
# define RPL_JOIN_WATCH(nick, user, host, channel) (":" + nick + "!" + user + "@" +  host + " JOIN " + channel + "\r\n")
# define ERR_BADCHANNELKEY(client, channel) (":irc.1337.ma 475 " + client + " " + channel + " :Cannot join channel (+k)\r\n")
# define ERR_CHANNELISFULL(client, channel) (":irc.1337.ma 471 " + client + " " + channel + " :Cannot join channel (+l)\r\n")
# define ERR_INVITEONLYCHAN(client, channel) (":irc.1337.ma 473 " + client + " " + channel + " :Cannot join channel (+i)\r\n")
# define ERR_CANTJOINCHANNEL(client, channel) (":irc.1337.ma " + client + " " + channel + " :Cannot join channel\r\n")
# define ERR_ALREADYJOINED(client, channel) (":irc.1337.ma " + client + channel + " :You have already joined this channel\r\n")

// KICK
# define ERR_CHANOPRIVSNEEDED(client, channel) (":irc.1337.ma 482 " + client +  channel + " :You're not channel operator\r\n")
# define RPL_KICK(client, username, channel, kicked, reason) (":" + client + "!" + username + "@irc.1337.ma" + " KICK " + channel + " " + kicked + " " + reason + "\r\n")

// MODE
# define RPL_CHANNELMODEIS(client, channel, mode, nada) (":irc.1337.ma 324 " + client + " " + channel + " " + mode + nada + "\r\n")
# define ERR_UMODEUNKNOWNFLAG(client) (":irc.1337.ma 501 " + client + " :Unknown MODE flag\r\n")
# define RPL_CREATIONTIME(client, channel, time) (":irc.1337.ma 329 " + client + " " + channel + " " + time + "\r\n")

// NICK
# define ERR_NONICKNAMEGIVEN(client) (":irc.1337.ma 431 " + client + " :There is no nickname.\r\n")
# define ERR_ERRONEUSNICKNAME(client, nickname) (":irc.1337.ma 432 " + client + " " + nickname + " :Erroneus nickname\r\n")
# define ERR_NICKNAMEINUSE(client, nickname) (":irc.1337.ma 433 " + client + " " + nickname + " :Nickname is already in use.\r\n")
# define RPL_NICK(oclient, uclient) (":" + oclient + "!" + uclient + "@irc.1337.ma NICK " + "\r\n")
# define ERR_NICKNAMESET(client) (":irc.1337.ma " + client + " :You already set a nickname!" + "\r\n")
# define ERR_NICKAUTHSET(client) (":irc.1337.ma " + client + " :You are already authenticated with nickname!" + "\r\n")
# define ERR_PASSWDFIRST(client) (":irc.1337.ma " + client + " :Please authenticate with password/nickname first!" + "\r\n")
# define ERR_PASSWDSET(client) (":irc.1337.ma " + client + " :You already authenticated with password!" + "\r\n")

// PASS
# define ERR_PASSWDMISMATCH(client) (":irc.1337.ma 464 " + client + " :Password incorrect.\r\n")

// PRIVMSG
# define ERR_NOSUCHNICK(client, target) (":irc.1337.ma 401 " + client + " " + target + " :No such nick/channel\r\n")
# define ERR_NORECIPIENT(client) (":irc.1337.ma 411 " + client + " :No recipient given PRIVMSG\r\n")
# define ERR_NOTEXTTOSEND(client) (":irc.1337.ma 412 " + client + " :No text to send\r\n")
# define PRIVMSG_TO_USER(nickname, username, host, target, message) (":" + nickname + "!" + username + "@" + host + " PRIVMSG " + target + " :" + message + "\r\n")
# define PRIVMSG_TO_CHANNEL(nickname, username, target, message) ":" + nickname + "!" + username + "@irc.1337.ma" + " PRIVMSG " + target + " :" + message + "\r\n"

// TOPIC
# define RPL_TOPIC(client, channel, topic) (":irc.1337.ma 332 " + client + " " + channel + " " + topic + "\r\n")
# define RPL_NOTOPIC(client, channel) (":irc.1337.ma 331 " + client + channel + " :No topic is set\r\n")
# define RPL_TOPICWHOTIME(client, channel, nickname, time) (":irc.1337.ma 333 " + client + " " + channel + " " + nickname + " " + time + "\r\n")

// USER
# define ERR_ALREADYREGISTERED(client) (":irc.1337.ma 462 " + client + " :You may not reregister.\r\n")

// QUIT
# define RPL_QUIT(nickname, username, message) (":" + nickname + "!" + username + "@irc.1337.ma" + " QUIT :" + message + "\r\n")

#endif