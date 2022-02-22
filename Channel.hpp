#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <string>

# include "Server.hpp"
# include "User.hpp"


class Server;

class Channel 
{
public:
	Channel(std::string const &name);
	~Channel();

	std::string		getName();
	std::string		getTopic() { return _topic;}
	int				addRegUser(User &user);
	int				addOperator(User &user);
	int				removeRegUser(User &user, int fl_quit);
	int				removeOperator(User &user);
	int				sendMessageToAll(Server &server, std::string const &str);
	std::string				getUsersList();
	void				sendMessage_chan(const std::string &message, User &user);
	std::vector<User *>		getRegUsers() {return _regularUsers;}
	std::vector<User *>		getOperators() {return _operators;}
	int					showTopic(User &user);
	void					setTopic(User &user, std::string &topic);


private:
	Channel();
	std::string				_topic;
	std::string				_name;
	std::vector<User *>		_operators;
	std::vector<User *>		_regularUsers;
};

#endif