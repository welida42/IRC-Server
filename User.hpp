#ifndef USER_HPP
# define USER_HPP

#include <iostream>
#include <string>
#include <queue>
#include <netinet/in.h>
#include <arpa/inet.h>
# include "Server.hpp"


class User
{
public:
	User(int fd, struct sockaddr_in client, std::string &server_name);
	~User();
	User &operator=( const User &rhs );

	void	printUser();
	int		getFD();
	std::string		getNick();
	void	setNick( std::string const &nickname);
	int		splitToMessage();
	//bool	getRegistered();
	void	sendMessage(std::string const &message);

	std::string					text;		//text from buffer
	std::vector<std::string>	message;
	std::string					command;
	std::string					adressee;	// for whom
	std::string					sender;		// who send
	std::string					cmd_string;
	bool						registered;	//is he registered
	bool						pass_ok;	//pass provided and correct
	std::string					nickname;	//user nickname, it should be unique
	std::string					host_name;
	std::string					server_name;
	std::string					real_name;
	std::string					user_name;
	std::string					away_text;	//away status with text
	struct 	sockaddr_in			client;		//client addr and port
	//void	addMessage(std::string message);
	std::vector<Channel *>		channels;
	bool 						_away;		//away status only
	bool						server_operator;

private:
	int						_pass;		//pass provided and correct
	//std::string				_nickname;	//user nickname, it should be unique
	//bool					_registered; //is he registered
	int						_fd;		//user used socket fd
	//struct 	sockaddr_in		_client;	//client addr and port
	//std::string				_text;		//text from buffer
	//std::vector<std::string>	_messages;	//messages from the user to proceed

	User();
};

#endif