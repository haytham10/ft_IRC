/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPL.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmokhtar <hmokhtar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/21 19:44:22 by hmokhtar          #+#    #+#             */
/*   Updated: 2023/09/21 19:44:22 by hmokhtar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RPL_HPP
#define RPL_HPP

#include "Client.hpp"
#include "IRCMessage.hpp"
#include "IRCServer.hpp"
#include "IRCChannels.hpp"


const std::string serverName = "Beta-Chad Server";
const int RPL_WELCOME = 001;
const int RPL_TOPIC = 332;
const int RPL_INVITING = 341;
const int ERR_UNKNOWNERROR = 400;
const int ERR_NOSUCHCHANNEL = 403;
const int ERR_CANNOTSENDTOCHAN = 404;
const int ERR_NORECIPIENT = 411;
const int ERR_NOTEXTTOSEND = 412;
const int ERR_UNKNOWNCOMMAND = 421;
const int ERR_NONICKNAMEGIVEN = 431;
const int ERR_ERRONEUSNICKNAME = 432;
const int ERR_NICKNAMEINUSE = 433;
const int ERR_USERNOTINCHANNEL = 441;
const int ERR_NOTONCHANNEL = 442;
const int ERR_USERONCHANNEL = 443;
const int ERR_NOTREGISTERED = 451;
const int ERR_NEEDMOREPARAMS = 461;
const int ERR_ALREADYREGISTERED = 462;
const int ERR_PASSWDMISMATCH = 464;
const int ERR_UNKNOWNMODE = 472;
const int ERR_INVITEONLYCHAN = 473;
const int ERR_BADCHANNELKEY = 475;
const int ERR_CHANOPRIVSNEEDED = 482;
const int ERR_UMODEUNKNOWNFLAG = 501;

# define RPL_WELCOME(nickname) (":irc.1337.ma 001 " + nickname + " :Welcome to `ZAZA Server` !" + "\r\n")

# define ERR_UNKNOWNCOMMAND(client, command) (":irc.1337.ma 421 " + client + " " + command + " :Unknown command\r\n")

# define ERR_NOTREGISTERED(client) (":irc.1337.ma 451 " + client + ": You have not registered." + "\r\n")

// INVITE
# define ERR_NEEDMOREPARAMS(client, command) (":irc.1337.ma 461 " + client + " " + command + " :Not enough parameters.\r\n")
# define ERR_NOSUCHCHANNEL(client, channel) (":irc.1337.ma 403 " + client + channel + " :No such channel\r\n")
# define ERR_NOTONCHANNEL(client, channel) (":irc.1337.ma 442 " + client + channel + " :The user is not on this channel.\r\n")
# define ERR_USERONCHANNEL(client, nick, channel) (":irc.1337.ma 443 " + client + " " + nick + channel + " :Is already on channel\r\n")
# define RPL_INVITING(user_id, client, nick, channel) (user_id + " 341 " + client + " " + nick + channel + "\r\n")
# define RPL_INVITE(user_id, invited, channel) (user_id + " INVITE " + invited + channel + "\r\n")

// JOIN
# define RPL_JOIN(nick, channel) (nick + " JOIN :" +  channel + "\r\n")
# define ERR_BADCHANNELKEY(client, channel) ("475 " + client + channel + " :Cannot join channel (+k)\r\n")

// KICK
# define ERR_USERNOTINCHANNEL(client, nickname, channel) ("441 " + client + " " + nickname + channel + " :They aren't on that channel\r\n")
// # define ERR_CHANOPRIVSNEEDED(client, channel) ("482 " + client +  channel + " :You're not channel operator\r\n")
# define RPL_KICK(user_id, channel, kicked, reason) (user_id + " KICK #" + channel + " " + kicked + " " + reason + "\r\n")

// MODE
/* user mode */
#define MODE_USERMSG(client, mode) (":" + client + " MODE " + client + " :" + mode + "\r\n")
#define ERR_UMODEUNKNOWNFLAG(client) (":irc.1337.ma 501 " + client + " :Unknown MODE flag\r\n")
#define ERR_USERSDONTMATCH(client) ("502 " + client + " :Cant change mode for other users\r\n")
#define RPL_UMODEIS(client, mode) (":irc.1337.ma 221 " + client + " " + mode + "\r\n")
/* channel mode */
#define MODE_CHANNELMSG(channel, mode) (":irc.1337.ma MODE #" + channel + " " + mode + "\r\n")
#define MODE_CHANNELMSGWITHPARAM(channel, mode, param) (":irc.1337.ma MODE #" + channel + " " + mode + " " + param + "\r\n")
#define RPL_CHANNELMODEIS(client, channel, mode) (":irc.1337.ma 324 " + client + channel + " " + mode + "\r\n")
#define RPL_CHANNELMODEISWITHKEY(client, channel, mode, password) (":irc.1337.ma 324 " + client + channel + " " + mode + " " + password + "\r\n")
#define ERR_CANNOTSENDTOCHAN(client, channel) ("404 " + client + channel + " :Cannot send to channel\r\n")
#define ERR_CHANOPRIVSNEEDED(client, channel) (":irc.1337.ma 482 " + client + channel + " :You're not channel operator\r\n")
#define ERR_INVALIDMODEPARAM(client, channel, mode, password) ("696 " + client + channel + " " + mode + " " + password + " : password must only contained alphabetic character\r\n")

// NICK
# define ERR_NONICKNAMEGIVEN(client) (":irc.1337.ma 431 " + client + " :There is no nickname.\r\n")
# define ERR_ERRONEUSNICKNAME(client, nickname) (":irc.1337.ma 432 " + client + " " + nickname + " :Erroneus nickname\r\n")
# define ERR_NICKNAMEINUSE(client, nickname) (":irc.1337.ma 433 " + client + " " + nickname + " :Nickname is already in use.\r\n")
# define RPL_NICK(oclient, uclient) (":" + oclient + "!" + uclient + "@irc.1337.ma NICK " + "\r\n")
# define ERR_NICKNAMESET(client) (":irc.1337.ma " + client + " :You already set a nickname!" + "\r\n")
# define ERR_NICKAUTHSET(client) (":irc.1337.ma " + client + " :You are already authenticated with nickname!" + "\r\n")
# define ERR_PASSWDFIRST(client) (":irc.1337.ma " + client + " :Please authenticate with password first!" + "\r\n")

// PASS
# define ERR_PASSWDMISMATCH(client) (":irc.1337.ma 464 " + client + " :Password incorrect.\r\n")
# define ERR

// PRIVMSG
# define ERR_NOSUCHNICK(client, target) ("401 " + client + " " + target + " :No such nick/channel\r\n")
# define ERR_NORECIPIENT(client) ("411 " + client + " :No recipient given PRIVMSG\r\n")
# define ERR_NOTEXTTOSEND(client) ("412 " + client + " :No text to send\r\n")
# define RPL_PRIVMSG(nick, username, target, message) (":" + nick + "!" + username + "@irc.1337.ma PRIVMSG " + target + " " + message + "\r\n")

// TOPIC
# define RPL_TOPIC(client, channel, topic) (":irc.1337.ma 332 " + client + " " + channel + " " + topic + "\r\n")
# define RPL_NOTOPIC(client, channel) (":irc.1337.ma 331 " + client + channel + " :No topic is set\r\n")

// USER
# define ERR_ALREADYREGISTERED(client) (":irc.1337.ma 462 " + client + " :You may not reregister.\r\n")

#endif