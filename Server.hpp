//#ifndef SERVER_HPP
//# define SERVER_HPP

#pragma once

# include <iostream>
# include <sstream>
# include <string>
# include <vector>
# include <map>

# include <unistd.h>
# include <poll.h>
# include <sys/socket.h>
# include <sys/fcntl.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/select.h>

class User;
class Server;
class Channel;

# include "Channel.hpp"
# include "User.hpp"

# include "Defines.hpp"
# define TCP 6 //From 'man protocols' -  /etc/protocols
# define BUFLEN 512
# define DISCONNECT	-2
# define SERVER_NAME "SERVER_NAME"

#ifdef __APPLE__
#define IRC_NOSIGNAL SO_NOSIGPIPE
#else
#define IRC_NOSIGNAL MSG_NOSIGNAL
#endif


typedef  int (Server::*Method) (User &);
int		sendError(User &user, int num, const std::string &arg1="", const std::string &arg2="");
int		sendReply(User &user, int num, const std::string &arg1="",
			const std::string &arg2="",const std::string &arg3="",
			const std::string &arg4="");

std::string	i_to_str(size_t number);

class Server
{
public:
	Server(int port, std::string password);
	~Server();
	//Server &operastor=( Server const & rhs );

	int	start();
	int	handleMessage(User &user);
	//int	handleMessage(User *user, char* message);
	//int	handleMessage(int fd, char* message);
	//int	handleMessage(int fd, std::string message);
	int		handleMessage(int fd);
	int		executeCommand(User &user);
	void	showMOTD(User &user);
	std::string		recText;
	//std::map<std::string, Method>		commands;


	//commands list
	int		pass(User &user);
	int		nick(User &user);
	int		user(User &user);
	int		oper(User &user);
	int		kill(User &user);

	int 	privmsg(User &user);
	int 	notice(User &user);
	int		quit(User &user);
	int		away(User &user);
	int		who(User &user);

	int		join(User &user);
	int 	invite(User &user);
	int		list(User &user);
	int		names(User &user);
	int 	topic(User &user);
	int 	mode(User &user);
	int 	ping(User &user);
	int 	part(User &user);
	int 	kick(User &user);

	int		readFromClient(User *user, int fd, std::vector<char> buf);
	void	writeToClient(int fd, std::string str);
	void	sendMessage(User &user, std::string const &message);
	void	sendMessage(int fd, std::string const &message);
	void	check_error(int num, std::string str);
	void	removeUser(User &user, int fl_quit); // fl_quit = 1 if need to perform part
	void	bot(User &user);
	User * find_user(std::string nick, std::vector<User *> users);
	
	//	std::string server_name = "SERVER_NAME";
	std::vector<User *>		_users;
private:
	int						_port;
	struct	sockaddr_in		_addr;
	std::string				_password;
	std::string				_server_name;
	std::string				_server_oper_user;
	std::string				_server_oper_pass;
	std::vector<pollfd>		_pollfds;
	//std::vector<User *>		_users;
	std::map<std::string, Channel *>	_channels;
	std::map<std::string, Method>		commands;
	fd_set	_active_set, _read_set;

	Server();
	Server( const Server &src );
	//Server &operastor=( Server const & rhs );
	//int		readFromClient(User *user, int fd, char *buf);
	//int		readFromClient(User *user, int fd, std::vector<char> buf);
	//int		readFromClient(int fd, char *buf);
	//void	writeToClient(int fd, std::string str);
	//void	sendMessage_chan(User &user, std::string const &message);
	//void	sendMessage_chan(int fd, std::string const &message);
	//void	writeToClient(int fd, char *buf);
	//void	check_error(int num, std::string str);
	//void	removeUser(User &user);
	//User * find_user(std::string nick, std::vector<User *> users);
	//Channel * find_channel(std::string channel_name);
};

//#endif