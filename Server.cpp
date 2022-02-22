#include "Server.hpp"
#include "User.hpp"

Server::Server(int port, std::string password):
			_port(port),
			_password(password)
{
	std::cout << "IRC server is initialized." << std::endl;
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(this->_port);
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_server_name = SERVER_NAME;
	_server_oper_user = "123";
	_server_oper_pass = "456";
	commands["PASS"] = &Server::pass;
	commands["NICK"] = &Server::nick;
	commands["USER"] = &Server::user;
	commands["OPER"] = &Server::oper;
	commands["KILL"] = &Server::kill;
	commands["QUIT"] = &Server::quit;
	commands["WHO"] = &Server::who;

	commands["PRIVMSG"] = &Server::privmsg;
	commands["AWAY"] = &Server::away;
	commands["NOTICE"] = &Server::notice;
	commands["JOIN"] = &Server::join;
	commands["LIST"] = &Server::list;
	commands["NAMES"] = &Server::names;
	commands["TOPIC"] = &Server::topic;
	commands["INVITE"] = &Server::invite;
	commands["MODE"] = &Server::mode;
	commands["PART"] = &Server::part;
	commands["KICK"] = &Server::kick;
	commands["PING"] = &Server::ping;
}

int Server::ping(User &user)
{
	(void) user;
	std::cout << "PONG\n";
	return 0;
}

Server::~Server()
{
	std::cout << "Thank you for using our product. GoodBye!" << std::endl;
}

int Server::start()
{
	int		fd, err, opt = 1;
	int		sock, new_sock;
	struct 	sockaddr_in		client;
	//char	buf[BUFLEN];
	std::vector<char> buf(BUFLEN);
	socklen_t	size;

	sock = socket(PF_INET, SOCK_STREAM, TCP);
	check_error(sock, "socket");
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof(opt));
	check_error(bind(sock,(struct sockaddr*)&_addr,sizeof(_addr)), "bind");
	check_error(listen(sock,42), "listen");
	FD_ZERO(&_active_set);
	FD_SET(sock, &_active_set);
	fcntl(sock, F_SETFL, O_NONBLOCK);
	while (1)
	{
		_read_set = _active_set;
		check_error(select(FD_SETSIZE,&_read_set,NULL,NULL,NULL), "select");
		//check new connections
		if (FD_ISSET(sock, &_read_set))
		{
			size = sizeof(client);
			new_sock = accept(sock,(struct sockaddr*)&client,&size);
			check_error(new_sock, "accept to new socket"); 
			std::cout << "Server: connect from host "
					<< inet_ntoa(client.sin_addr) << " port "
					<< ntohs(client.sin_port) << std::endl;
			FD_SET(new_sock, &_active_set);
			_users.push_back(new User(new_sock, client, _server_name));
			// next message need to rewrite according with standard: :SERVER_NAME 372 etc
			//sendMessage_chan(new_sock, "Welcome to WElida & DTawney IRC Server ;-)");
			std:: string txt = ":SERVER_NAME 381 :Welcome to WElida & DTawney IRC Server ;-)";
			sendMessage(new_sock, txt);
			continue;
		}
		//check connected users
		for (int i = 0; i < (int)_users.size(); i++)
		{
			fd = _users[i]->getFD();
			if (FD_ISSET(fd, &_read_set))
			{
				err = Server::readFromClient(_users[i], fd, buf);
				if (err<0) {
					close (fd);
					FD_CLR(fd,&_active_set);
					//erase
					std::cout << "User '" << _users[i]->getNick() << "' disconnected." << std::endl;
					removeUser(*_users[i], 1);
				} else {
					//Server::handleMessage(fd, buf);
					if (_users[i]->text.find(10) != std::string::npos)
					{
						Server::handleMessage(*_users[i]);
						_users[i]->text.clear();
					}
				}
				//_users[i]->text.erase();
			}
		}
	}
}

/*int		Server::readFromClient(User *user, int fd, std::vector<char> buf)
{
	int		nbytes;

	nbytes = recv(fd, buf.data(), buf.size(), 0);
	user->text.clear();
	if (nbytes < 0) {
		std::cout << "Server: recv failure" << std::endl;
		return -1;
	} else if (nbytes == 0) {
		return -1;
	} else {
		if (buf.size() > 0)
		{
			user->text += buf.data();
			std::cout << "------------" << std::endl;
			while (nbytes > 0 && strlen(buf.data()) > 0)
			{
				user->text += buf.data();
				//std::cout << "Current buf size: " << strlen(buf.data()) << ", for text: " << buf.data() << std::endl;
				//std::cout << "Current size: " << user->text.length() << " for text: " << std::endl;
				nbytes = recv(fd, buf.data(), buf.size(), 0);
			}
			// dividing message to 2 by text size
			user->text = user->text.substr(0, (int)user->text.size()/2);*/
			// temporary disable splitting message to 2 by enter
			//user->text = user->text.substr(0, user->text.find((char)10));
			//std::cout << "Server got message size: " << user->text.length() << ", text: " << user->text << std::endl;
			//std::cout << "------------" << std::endl;
			
			/*for(int i = 0; i < (int)user->text.length(); i++)
			{
				std::cout << "Pos " << i << ", symbol code:  " << (int)user->text[i] << std::endl;
			}*/
		/*}
		return 0;
	}
}*/

int		Server::readFromClient(User *user, int fd, std::vector<char> buf)
{
	int		nbytes;
	char	buffer;

	nbytes = recv(fd, &buffer, 1, 0);
	buf.size();
	if (nbytes < 0) {
		std::cout << "Server: recv failure" << std::endl;
		return -1;
	} else if (nbytes == 0) {
		return -1;
	} else
	{
		if (buffer == 13)
		{}
		else if (buffer != 10 && buffer != 0)
			user->text.push_back(buffer);
		else
			user->text.push_back(10);
	}
	return 0;
}

void	Server::writeToClient(int fd, std::string instr)
{
	int				nbytes = 0;
	//unsigned char	*s;
	size_t 			len = BUFLEN;

	const char *str = "answer\n";
	//for (s=(unsigned char*)buf; *s; s++) *s = toupper(*s);
	if (instr.size() < 512)
		len = instr.size() + 1;
	//nbytes = send(fd, buf, len, IRC_NOSIGNAL);
	if (instr.size() > 0)
		nbytes = send(fd, str, 7, IRC_NOSIGNAL);
	std::cout << "Send back: " //<< buf 
			<< "nbytes = " << nbytes << std::endl;
	if (nbytes<0)
		std::cout << "Server: send failure" << std::endl;
}

void	Server::sendMessage(User &user, std::string const &message)
{
	int		nbytes;
	std::string message_to = message;
	message_to.push_back('\n');
	//need also to send from whom the message and another attributes
	if (message.length() > 0)
		nbytes = send(user.getFD(), message_to.data(), message_to.length(), IRC_NOSIGNAL);
}

void	Server::sendMessage(int fd, std::string const &message)
{
	int		nbytes;
	std::string message_to = message;
	message_to.push_back('\r');
	message_to.push_back('\n');
	//need also to send from whom the message and another attributes
	if (message.length() > 0)
	{
		nbytes = send(fd, message_to.data(), message_to.length(), IRC_NOSIGNAL);
		std::cout << "exec Send_message to fd=" << fd << ", message:" << message_to.data(); // << std::endl;
		//message_to = ":dt!dt@127.0.0.1 PRIVMSG assa :test message";
		//message_to = "PRIVMSG assa :test message";
		//message_to.push_back('\n');
		//nbytes = send(fd, message_to.data(), message_to.length(), IRC_NOSIGNAL);
	}
}

void	Server::check_error(int num, std::string str)
{
	if (num < 0)
	{
		std::cout << "An issue with " << str << std::endl;
		exit(-1);
	}
}

int	Server::handleMessage(User &user)
{
	int		fd = user.getFD();
	std::cout << "From '" << fd << "' message size: " << user.text.size() << ", text:" << user.text << std::endl;
	//user->splitToMessage();
	// start -- process received text/message in cycle by spliting '\r\n'
	this->recText = user.text;
	int i = 0;
	int textStart = 0;
	int	maxI = recText.size();
	//std::cout << "Text/message size: " << maxI << std::endl;
	while (i < maxI)
	{
		if (recText[i] == '\r' && recText[i + 1] == '\n')
		{
			std::cout << "\\r\\n position: " << i << std::endl;
			user.text = recText.substr(textStart, i - textStart);
			std::cout << "text after substr: " << user.text.data() << " --"<< std::endl;
			if (user.splitToMessage() == 0)
				Server::executeCommand(user);
			i = i + 1;
			textStart = i + 1;
		}
		//if (recText[i] == '\n' && recText[i + 1] == '\r')
		//	std::cout << "\\n\\r position: " << i << std::endl;
		i++;
	}
	if (textStart < maxI)
	{
		user.text = recText.substr(textStart, i - textStart);
		if (user.text.back() == '\n')
			user.text.pop_back();
		if (user.splitToMessage() == 0)
				Server::executeCommand(user);
	}

	// end -- process received text/message in cycle by spliting '\r\n'

	//if (user.splitToMessage() == 0)
	//	Server::executeCommand(user);
	if (user.text == "stop") {
		close(fd);
		FD_CLR(fd,&_active_set);
	} else if (user.text == "list")
	{
		for (int i = 0; i < (int)_users.size(); i++) {
			_users[i]->printUser();
		}
	} /*else {
		Server::writeToClient(fd,user->text);
	}*/
	user.message.clear();
	return (0);
}

int Server::executeCommand(User &user)
{
	// if a command is not in PASS NICK USER QUIT and the client is not registered
	std::cout << "Command: " << user.command 
				<< " " << user.registered 
				<< " " << user.command.compare("PASS") 
				<< " " << user.command.compare("NICK") 
				<< " " << user.command.compare("USER")
				<< std::endl;
	if (user.registered == 0 && 
			(user.command.compare("PASS") != 0 && 
			user.command.compare("NICK") != 0 && 
			user.command.compare("USER") != 0 &&
			user.command.compare("QUIT") ))
	{
		this->sendMessage(user, "You are not registerd");
		sendError(user, ERR_NOTREGISTERED);
	}
	try
	{
		int ret = (this->*(commands.at(user.command)))(user);
		//if (ret == DISCONNECT)
		//	return (DISCONNECT);
		if (ret != 0)
			std::cout << "ret: " << ret << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cout << "No command (( " << std::endl;
		//sendError(user, ERR_UNKNOWNCOMMAND, msg.getCommand());
	}
	user.message.clear();
	user.command = "";
	user.sender = "";
	user.adressee = "";
	user.cmd_string = "";
	return (0);
}

//commands from message
// надо отовсюду удалить! со всех каналов?!
int		Server::quit(User &user)
{
	if (!user.adressee.empty())
		sendMessage(user, user.adressee + "\n");
	close(user.getFD());
	FD_CLR(user.getFD(),&_active_set);
	std::cout << "User with FD = " << user.getFD() << " exit from server." << std::endl;
	removeUser(user, 1);
	return (0);
}

int		Server::pass(User &user)
{
	//std::cout << "--ex PASS: " << user.adressee.data() << " -- " << user.message[1] << std::endl;
	if (user.adressee.empty())
		sendError(user, ERR_NEEDMOREPARAMS, user.command);
	if (user.pass_ok == true)
		sendError(user, ERR_ALREADYREGISTRED);
	if (user.message[1][0] == ':')
	{
		user.message[1].erase(0, 1);
		//std::cout << "--ex PASS user.message[1] after erase: " << user.message[1] << " -- " << std::endl;
	}
	if (user.message[1].compare(this->_password) == 0 )
	{
		user.pass_ok = true;
		std::cout << "User with fd=" << user.getFD() << " provided correct password." << std::endl;
	}
	return (0);
}

int		Server::nick(User &user)
{
	std::string nick = user.adressee;
	//std::cout << "exec NICK start for " << user.adressee << std::endl;
	if (nick.empty())
	{
		sendError(user, ERR_NEEDMOREPARAMS, user.command);
		return (0);
	}
	//std::cout << "exec NICK checked for empty string: " << nick << std::endl;
	if (user.pass_ok)
	{
		std::string nickname = user.message[1];
		//std::cout << "exec NICK before cycle: " << nick << " " << nickname << std::endl;
		if (nickname.length() > 0)
		{
			//std::cout << "exec NICK before cycle2: " << nickname << std::endl;
			std::vector<User *>::iterator  it = _users.begin();
			std::vector<User *>::iterator  ite = _users.end();
			for (; it != ite; it++ )
			{
				if ((*it)->getNick() == nickname && user.getFD() != (*it)->getFD())
				{
						sendError(user, ERR_NICKNAMEINUSE, nick);
						std::cout << "Provided NICK is not unique: " << nick << " <--end" << std::endl;
						return (-1);
				}
			}
			if (user.getNick().size() == 0 && user.user_name.size() > 0)
			{
				user.setNick(nickname);
				showMOTD(user);
				std::cout << "-- A new client is registered now with nick " << user.getNick() << " <--end" << std::endl;
			}
			else
				user.setNick(nickname);
		}
	}
	else
	{
		sendMessage(user, "Please provide the server password first");
	}
	return (0);
}

int		Server::oper(User &user)
{
	//	:replies => [RPL_YOUREOPER] 
    //	:errors  => [ERR_NEEDMOREPARAMS, ERR_NOOPERHOST, ERR_PASSWDMISMATCH]
	//std::cout << "exec OPER start: " << user.getNick() << std::endl;
	if (user.message.size() < 3)
	{
		std::cout << "exec OPER: ERR_NEEDMOREPARAMS" << std::endl;
		sendError(user, ERR_NEEDMOREPARAMS, user.command);
		return (0);
	}
	if (user.message[1].compare(this->_server_oper_user) != 0 || 
			user.message[2].compare(this->_server_oper_pass) != 0)
	{
		std::cout << "exec OPER: ERR_PASSWDMISMATCH" << std::endl;
		sendError(user, ERR_PASSWDMISMATCH, user.command);
		return (0);
	}
	std::cout << "exec OPER finish with success: RPL_YOUREOPER for " 
				<< user.getNick() << std::endl;
	user.server_operator = true;
	sendReply(user, RPL_YOUREOPER);
	std::string format;
	format = ":" + user.nickname + "!" + user.user_name + "@" + user.host_name
			 + " MODE " + user.getNick() + " +o";
	std::vector<User *>::iterator it = _users.begin();
	std::vector<User *>::iterator end = _users.end();
	for (; it != end; ++it)
			(*it)->sendMessage(format);
	return (0);
}

int		Server::kill(User &user)
{
	//:errors  => [ERR_NOPRIVILEGES, ERR_NEEDMOREPARAMS, ERR_NOSUCHNICK,
    //                ERR_CANTKILLSERVER]
	if (!user.server_operator)
	{
		std::cout << "exec OPER: ERR_NOPRIVILEGES" << std::endl;
		sendError(user, ERR_NOPRIVILEGES, user.command);
		return (0);
	}
	if (user.message.size() < 2)
	{
		std::cout << "exec OPER: ERR_NEEDMOREPARAMS" << std::endl;
		sendError(user, ERR_NEEDMOREPARAMS, user.command);
		return (0);
	}
	User *tmp_user = find_user(user.message[1].data(), _users);
	if (!tmp_user)
	{
		std::cout << "exec OPER: ERR_NOSUCHNICK" << std::endl;
		sendError(user, ERR_NOSUCHNICK, user.command);
		return (0);
	}
	std::cout << "exec KILL with user:" << tmp_user->getNick() << std::endl;
	std::string format;
	format = ":" + user.nickname + "!" + user.user_name + "@" + user.host_name
			 + " " + user.text;
	std::vector<User *>::iterator it = _users.begin();
	std::vector<User *>::iterator end = _users.end();
	for (; it != end; ++it)
			(*it)->sendMessage(format);
	close(tmp_user->getFD());
	FD_CLR(tmp_user->getFD(),&_active_set);
	removeUser(*tmp_user, 1);
	return (0);
}

void	Server::showMOTD(User &user)
{
	sendReply(user, RPL_MOTDSTART, user.server_name);
//	sendMessage_chan(user, "You are successfully registered in the server.");
	std::string motd = "You are successfully registered in the server.";
	sendReply(user, RPL_MOTD, motd);
	sendReply(user, RPL_ENDOFMOTD);
}

int		Server::user(User &user)
{
	//if (user.pass_ok && user.nickname.size() > 0)
	if (user.pass_ok)
	{
		if (user.message.size() < 5)
		{
			sendError(user, ERR_NEEDMOREPARAMS, user.command);
			std::cout << "Text from USER: " << user.text << " <--end" << std::endl;
		}
		else if(user.registered == true)
			sendError(user, ERR_ALREADYREGISTRED);
		else 
		{
			if (user.getNick().size() > 0)
			{
				user.registered = true;
				user.user_name = user.message[1];
				user.real_name = user.message[4];
				showMOTD(user);
				std::cout << "exec USER is a successfull with NICK " << std::endl;
			}
			else
			{
				user.user_name = user.message[1];
				user.real_name = user.message[4];
				std::cout << "exec USER is a successfull without NICK " << std::endl;
			}
		}
	}
	return (0);
}

User * Server::find_user(std::string nick, std::vector<User *> users)
{
	std::vector<User *>::iterator  it = users.begin();
	std::vector<User *>::iterator  ite = users.end();
	for (; it != ite; it++ )
	{
		if ((*it)->getNick() == nick)
			return (*it);
	}
	return nullptr;
}


void	Server::removeUser(User &user, int fl_quit)
{
	// remove user from all channels and then remove provided user
	if (!_channels.empty())
	{
		std::map<std::string, Channel *>::iterator  it = _channels.begin();
		std::map<std::string, Channel *>::iterator  ite = _channels.end();
		for (; it != ite; it++ )
		{
			std::cout << "-- channel remove before 1 if" <<std::endl;
			if (_channels.empty())
				break;
			if (it->second->removeOperator(user) < 0)
			{
				std::cout << "exec removeOperator -- remove channel: " << it->second->getName() <<std::endl;
				delete (it->second);
				_channels.erase(it);
				std::cout << "-- channel removed " << std::endl;
				continue;
			}
			std::cout << "-- channel removed before 2 if" <<std::endl;
			if (it->second->removeRegUser(user, fl_quit) < 0)
			{
				std::cout << "exec removeRegUser -- remove channel: " << it->second->getName() << std::endl;
				delete (it->second);
				_channels.erase(it);
				std::cout << "-- channel removed " << std::endl;
			} 
		}
	}
	if (!_users.empty())
	{
		//_users.erase(&user);
		std::vector<User *>::iterator  it = _users.begin();
		std::vector<User *>::iterator  ite = _users.end();
		for (; it != ite; it++ )
		{
			if ((*it)->getNick() == user.getNick())
			{
				delete (*it);
				_users.erase(it);
				//break;
			}
		}
	}
}

void	User::sendMessage(std::string const &message)
{
	int		nbytes;
	std::string message_to = message;
	message_to.push_back('\n');
	//need also to send from whom the message and another attributes
	if (message.length() > 0)
		nbytes = send(getFD(), message_to.data(), message_to.length(), IRC_NOSIGNAL);
}

void	Channel::sendMessage_chan(const std::string &message, User &user)
{
	std::string format;
	format = ":" + user.nickname + "!" + user.user_name + "@" + user.host_name
			 + " " + message;
	std::vector<User *>::iterator it = _regularUsers.begin();
	std::vector<User *>::iterator end = _regularUsers.end();
	if (user.command == "PRIVMSG")
	{
		for (; it != end; ++it)
		{
			if ((*it)->_away)
				sendReply(user, RPL_AWAY, (*it)->nickname,
						  (*it)->away_text);
			if (*it != &user)
				(*it)->sendMessage(format);
		}
	}
	else
		for (; it != end; ++it)
			(*it)->sendMessage(format);
}

//sendMessage_chan("MODE " + this->name + " +o "  + users[0]->getNickname() + "\n", user, true);
int 	Server::mode(User &user)
{
	User *adresant;
	Channel *chan;
	std::string	flag;

	if (user.adressee.empty())
		return (sendError(user, ERR_NEEDMOREPARAMS, user.command));
	if (user.adressee.size() > 1 && user.adressee[0] == '#')
	{
		if (_channels.find(user.adressee)  != _channels.end())
			chan = _channels.find(user.adressee)->second;
		else
			return sendError(user, ERR_NOSUCHCHANNEL, user.adressee);
		if (!find_user(user.getNick(), chan->getOperators()))
			return sendError(user, ERR_CHANOPRIVSNEEDED, user.adressee);
		 if (find_user(user.getNick(), chan->getRegUsers()) == NULL)
			return sendError(user, ERR_NOTONCHANNEL, user.adressee);
		if (user.cmd_string.empty())
			return sendReply(user, RPL_CHANNELMODEIS, user
			.adressee,"n");
		else
			flag = user.message[2];

		if (flag == "+o")
		{
			if (user.message.size() < 4)
				return (sendError(user, ERR_NEEDMOREPARAMS, user.command));
			adresant = find_user(user.message[3], chan->getRegUsers());
			if (adresant == NULL)
				return sendError(user, ERR_NOSUCHNICK,user.message[3]);
			chan->addOperator(*adresant);
		}
		else if (flag == "-o")
		{
			if (user.message.size() < 4)
				return (sendError(user, ERR_NEEDMOREPARAMS, user.command));
			adresant = find_user(user.message[3], chan->getRegUsers());
			if (adresant == NULL)
				return sendError(user, ERR_NOSUCHNICK,user.message[3]);
			chan->removeOperator(*adresant);
		}
		else
			return sendError(user, ERR_UNKNOWNMODE, flag);

		chan->sendMessage_chan("MODE " + user.adressee + " " + flag + " " +
							   adresant->nickname, user);
	}
	else
	{
		if (user.adressee != user.nickname)
			return sendError(user, ERR_USERSDONTMATCH);
		if (user.cmd_string.empty() && user.server_operator)
			return sendReply(user, RPL_UMODEIS, "+o");
		else if (user.cmd_string.empty() && !user.server_operator)
			return sendReply(user, RPL_UMODEIS, "+");
		else
		{
			flag = user.message[2];
			if (flag == "+o")
				return 0;
			else if (flag == "-o")
				user.server_operator = false;
			else
				return sendError(user, ERR_UMODEUNKNOWNFLAG);
		}
	}
	return 0;
}

int 	Server::privmsg(User &user)
{
	User * adresant;
	Channel * ch;

	if (user.adressee.compare("bot") == 0)
	{
		bot(user);
		return (0);
	}
	if (user.adressee.empty())
		return (sendError(user, ERR_NORECIPIENT, user.command));
	if (user.cmd_string.empty())
		return (sendError(user, ERR_NOTEXTTOSEND));

	if (user.adressee[0] == '#' || user.adressee[0] == '&')
	{
		if (_channels.find(user.adressee)  != _channels.end())
			ch = _channels.find(user.adressee)->second;
		else
			return (sendError(user, ERR_NOSUCHNICK, user.adressee));
		if (!find_user(user.getNick(), ch->getRegUsers()))
			return (sendError(user, ERR_CANNOTSENDTOCHAN, ch->getName()));
		else
		{
			user.cmd_string = user.command + " " + ch->getName() + " " + ":"
					+ user.cmd_string;
			ch->sendMessage_chan(user.text, user);
		}
	}
	else
	{
		std::cout << "exec privmsg " << user.adressee << " " << user.cmd_string
				  << std::endl;
		adresant = find_user(user.adressee, _users);
		if (adresant == nullptr)
		{
			std::cout << "-- user: " << user.adressee << " don't find in _users" << std::endl;
			return (sendError(user, ERR_NOSUCHNICK, user.adressee));
		}
		user.cmd_string = ":" + user.getNick() + "!" + user.user_name + "@" +
			user.host_name + " " + user.text;
		if (adresant->_away)
			sendReply(user, RPL_AWAY, adresant->nickname,
					  adresant->away_text);
		sendMessage(adresant->getFD(), user.cmd_string);
	}
	return (0);
}

int 	Server::notice(User &user)
{
	User * adresant;
	Channel * ch;

	if (user.adressee.empty())
		return (sendError(user, ERR_NORECIPIENT, user.command));
	if (user.cmd_string.empty())
		return (sendError(user, ERR_NOTEXTTOSEND));

	if (user.adressee[0] == '#' || user.adressee[0] == '&')
	{
		if (_channels.find(user.adressee)  != _channels.end())
			ch = _channels.find(user.adressee)->second;
		else
			return (sendError(user, ERR_NOSUCHNICK, user.adressee));
		if (!find_user(user.getNick(), ch->getRegUsers()))
			return (sendError(user, ERR_CANNOTSENDTOCHAN, ch->getName()));
		else
		{
			user.cmd_string = user.command + " " + ch->getName() + " " + ":"
							  + user.cmd_string;
			ch->sendMessage_chan(user.cmd_string, user);
		}
	}
	else
	{
		std::cout << "exec privmsg " << user.adressee << " " << user.cmd_string
				  << std::endl;
		adresant = find_user(user.adressee, _users);
		if (adresant == nullptr)
		{
			std::cout << "-- user: " << user.adressee << " don't find in _users" << std::endl;
			return (sendError(user, ERR_NOSUCHNICK, user.adressee));
		}
		user.cmd_string = ":" + user.getNick() + "!" + user.user_name + "@" +
						  user.host_name + " " + user.text;
		sendMessage(adresant->getFD(), user.cmd_string);
	}
	return (0);
}

int 	Server::away(User &user)
{
	// need to save all starting from :
	//user.away_text = user.adressee;
	if (user.adressee.empty())
	{
		user._away = false;
		sendReply(user, RPL_UNAWAY);
	}
	else
	{
		user._away = true;
		user.away_text = user.text.substr(user.text.find(':') + 1);
		sendReply(user, RPL_NOWAWAY);
	}
	return (0);
}

int 	Server::invite(User &user)
{
	Channel *chan;

	if (user.cmd_string.empty())
		return sendError(user, ERR_NEEDMOREPARAMS, user.command);
	User *adresant = find_user(user.adressee, _users);
	if (adresant == NULL)
		return sendError(user, ERR_NOSUCHNICK, user.adressee);
	try
	{
		chan = _channels.at(user.message[2]);
		std::cout << "--try" << std::endl;
	}
	catch(const std::exception& e)
	{
		return sendError(user, ERR_NOTONCHANNEL, user.message[2]);
		std::cout << "--catch" << std::endl;
	}
	if (find_user(user.nickname, chan->getRegUsers()) == NULL)
		return sendError(user, ERR_NOTONCHANNEL, user.message[2]);
	if (find_user(adresant->nickname, chan->getRegUsers()))
		return sendError(user, ERR_USERONCHANNEL, adresant->nickname, chan->getName());
// is operator checking?
//	chan->addRegUser(*adresant);
//	chan->sendMessag()
	adresant->sendMessage(":"+user.getNick()+"!"+user.user_name+"@"+user
		.host_name + " " + user.command + " " + adresant->nickname + " :" +
		chan->getName());
	sendReply(user, RPL_INVITING, chan->getName(),
			  adresant->nickname);
	if (adresant->_away)
		sendReply(user, RPL_AWAY, adresant->server_name,
				  adresant->away_text);
	return (0);
}

int 	Server::join(User &user)
{
	std::cout << "-- join start --" << std::endl;
	if (user.adressee.empty())
		sendError(user, ERR_NEEDMOREPARAMS, user.command);
	if (user.adressee.size() > 1 && (user.adressee[0] == '#' || user.adressee[0] == '&'))
	{
		std::string channel_name = user.adressee;
//		channel_name.erase(0,1); //remove # or & from a channel name start
		std::cout << "-- channel name: " << channel_name << std::endl;
		std::cout << "try -- catch" << std::endl;
		try
		{
			//check that user did't registered here previosly
			_channels.at(channel_name);
			if (find_user(user.nickname, _channels.at(channel_name)
			->getRegUsers()) == NULL)
				(_channels.at(channel_name))->addRegUser(user);
			else
			{
				std::cout << "USER already in channel\n";
				return 0;
			}
			user.channels.push_back(_channels.at(channel_name));
			std::cout << "--try" << std::endl;
		}
		catch(const std::exception& e)
		{
			Channel *new_channel = new Channel(channel_name);
			_channels[channel_name] = new_channel;
			new_channel->addOperator(user);
			new_channel->addRegUser(user);
			user.channels.push_back(new_channel);
			std::cout << "--catch" << std::endl;
		}
		// send info messages and reply
//		std::string mess = ":" + user.getNick() + "!" + user.user_name + "@" + user.host_name + " ";
		//(_channels.at(channel_name))->sendMessageToAll(*this, mess + user.text );
		Channel *channel =  _channels.at(channel_name);
		channel->sendMessage_chan("JOIN :" + channel_name, user);//\n
		channel->showTopic(user);
		sendReply(user, RPL_NAMREPLY, "= " + channel_name, channel->getUsersList());
		sendReply(user, RPL_ENDOFNAMES, channel->getName());
//		mess = channel->getUsersList();
//		std::string mess2 = "= " + (_channels.at(channel_name))->getName();
//		sendReply(_server_name, user, RPL_NAMREPLY, mess2, mess);
//		mess = (_channels.at(channel_name))->getName();
//		sendReply(_server_name, user, RPL_ENDOFNAMES, mess);
	}
	else
		sendError(user, ERR_NOSUCHCHANNEL, user.adressee);
	return (0);
}

int		Server::who(User &user)
{
	if (user.adressee.empty() || user.adressee == "*")
	{
		for (size_t i = 0; i < _users.size(); ++i)
		{
				sendReply(user, RPL_WHOREPLY, "* " +
					_users[i]->server_name + " " + _users[i]->host_name + " " +
					_users[i]->server_name + " " + _users[i]->nickname + " H :0 " + _users[i]->real_name);
		}
	}
	else if (User *us = find_user(user.adressee, _users))
	{
		sendReply(user, RPL_WHOREPLY, "* " +
			us->server_name + " " + us->host_name + " " +
			us->server_name + " " + us->nickname + " H :0 " + us->real_name);
	}
	else
		std::cout << "WHO------------------\n";
	return (sendReply(user, RPL_ENDOFWHO, user.adressee));
}

int 	Server::names(User &user)
{
		if (user.adressee.empty())
		{
			std::map<std::string, Channel *>::iterator  it = _channels.begin();
			std::map<std::string, Channel *>::iterator  ite = _channels.end();
			for (; it != ite; it++)
			{
				sendReply(user, RPL_NAMREPLY, "= " +
					it->second->getName(),it->second->getUsersList());
			}

			std::vector<User *>::iterator itb = _users.begin();
			std::vector<User *>::iterator itend = _users.end();
			std::string without = "";
			ite = _channels.end();
			for (; itb != itend; itb++)
			{
				it = _channels.begin();
				int count = 0;
				for (; it != ite; it++)
				{
					if (find_user((*itb)->getNick(), it->second->getRegUsers()))
						count++;
				}
				if (count == 0) //((*itb)->channels.empty())
				{
					without+=(*itb)->nickname;
					if ((itb + 1) != itend)
						without+=" ";
				}
			}
			sendReply(user, RPL_NAMREPLY, "= *", without);
			sendReply(user, RPL_ENDOFNAMES, "*");
		}
		else
		{
			try
			{
				Channel	*chan = _channels.at(user.adressee);
				sendReply(user, RPL_NAMREPLY, "= " + chan->getName(),
						  chan->getUsersList());
				sendReply(user, RPL_ENDOFNAMES,chan->getName());
			}
			catch(const std::exception& e)
			{
				std::cout << "NO SUCH CHANNEL\n";
			}
		}
	return (0);
}

int 	Server::list(User &user) // list of chanells
{
	if (user.adressee.empty())
	{
		std::string chan_flags = "[+n]";
		std::map<std::string, Channel *>::iterator it = _channels.begin();
		std::map<std::string, Channel *>::iterator ite = _channels.end();
		sendReply(user, RPL_LISTSTART);
		for (; it != ite; it++)
		{
			sendReply(user, RPL_LIST, it->second->getName(),
					  i_to_str(it->second->getRegUsers().size()),
					  chan_flags + it->second->getTopic());
		}
		sendReply(user, RPL_LISTEND);
	}
	else
		try
		{
			std::string chan_flags = "[+n]";
			Channel	*chan = _channels.at(user.adressee);
			sendReply(user, RPL_LISTSTART);
			sendReply(user, RPL_LIST, chan->getName(),
					  i_to_str(chan->getRegUsers().size()),
					  chan_flags + chan->getTopic());
			sendReply(user, RPL_ENDOFNAMES,chan->getName());
		}
		catch(const std::exception& e)
		{
			std::cout << "NO SUCH CHANNEL\n";
		}

	return (0);
}

int	Channel::showTopic(User &user)
{
	if (_topic.empty())
		return sendReply(user, RPL_NOTOPIC, getName());
	else
		return sendReply(user, RPL_TOPIC, getName(), _topic);
}

void	Channel::setTopic(User &user, std::string &topic)
{
	_topic = topic;
	sendMessage_chan("TOPIC " + getName() + " :" + _topic, user);
}

int 	Server::topic(User &user)
{
	std::string channel_name = user.adressee;
	if (channel_name.empty())
		return sendError(user, ERR_NEEDMOREPARAMS, user.command);

	try
	{
		Channel	*channel = _channels.at(channel_name);
		if (!find_user(user.getNick(), channel->getRegUsers()))
			return sendError(user, ERR_NOTONCHANNEL, channel_name);//if args=1
			 // -show topic, if args=2 - change it to 2. args needto split
		if (user.cmd_string.empty())
			return channel->showTopic(user);
		else if (!find_user(user.getNick(), channel->getOperators()))
			return sendError(user, ERR_CHANOPRIVSNEEDED, channel_name);
		channel->setTopic(user, user.cmd_string);
		std::cout << "--try" << std::endl;
	}
	catch(const std::exception& e)
	{
		sendError(user, ERR_NOTONCHANNEL, channel_name);
		std::cout << "--catch" << std::endl;
	}
	return 0;
}

int 	Server::part(User &user)
{
// go out from mentioned channel/channels	
// PART #oz-ops,#group5
// ERR_NEEDMOREPARAMS, ERR_NOSUCHCHANNEL, ERR_NOTONCHANNEL
	int j = 0;
	if (user.message.size() > 2 && user.message[1][0] == ':' && (user.message[2][0] == '&' || user.message[2][0] == '#'))
		j = 2;
	else if (user.message.size() > 1 && (user.message[1][0] == '#' || user.message[1][0] == '&'))
		j = 1;
	else
	{
		sendError(user, ERR_NEEDMOREPARAMS, "PART");
		return (1);
	}
	std::string chList = user.message[j];
	int maxI = chList.size();
	int textStart = 0;
	std::string channelName;
	Channel *ch;
	for (int i = 0; i <= maxI; i++)
	{
		//std::cout << "--pos " << i << " -- symbol:" << chList[i] << "--" << std::endl;
		if (chList[i] == ',' || chList[i] == 0)
		{
			channelName = chList.substr(textStart, i - textStart);
			//std::cout << "Channel name after substr: " << channelName.data() << " --" << i << std::endl;
			if (chList[i] == ',')
				i = i + 1;
			textStart = i;
			// try to remove from channel
			// check is that channel exist
			// check that user is in channel
			// if exist delete user from reg_user and operators
			// if he was a last user the channel should be removed
			if (_channels.find(channelName)  != _channels.end())
			{
				ch = _channels.find(channelName)->second;
				std::map<std::string, Channel *>::iterator  it = _channels.find(channelName);
				// check/find user in regUsers in channel
				if (!find_user(user.getNick(), ch->getRegUsers()))
				{
					sendError(user, ERR_NOTONCHANNEL, channelName);
					continue;	
				}
				if (ch->removeOperator(user) < 0)
				{
					std::cout << "exec removeOperator -- remove channel: " << ch->getName() <<std::endl;
					delete (it->second);
					_channels.erase(it);
					std::cout << "-- channel removed " << std::endl;
					continue;
				}
				std::cout << "-- channel removed before 2 if" <<std::endl;
				if (ch->removeRegUser(user, 0) < 0)
				{
					std::cout << "exec removeRegUser -- remove channel: " << ch->getName() << std::endl;
					delete (it->second);
					_channels.erase(it);
					std::cout << "-- channel removed " << std::endl;
					continue;
				}
				std::string mess = ":" + user.getNick() + "!" + user.user_name + "@" + user.host_name + " ";
				ch->sendMessageToAll(*this, mess + user.text);
			}
			else 
				sendError(user, ERR_NOSUCHCHANNEL, channelName);
		}
	}
	//if (textStart
	return (0);
}

int 	Server::kick(User &user)
{
//	operator can kick another users from channel/channels
//	KICK #Finnish John :Speaking English
// ERR_NEEDMOREPARAMS, ERR_NOSUCHCHANNEL, ERR_BADCHANMASK, ERR_CHANOPRIVSNEEDED, ERR_NOTONCHANNEL
// need to send message to all channel users except gone
	int j = 2;
	if (user.message.size() > 3 && user.message[1][0] == ':' && (user.message[2][0] == '&' || user.message[2][0] == '#'))
		j = 2;
	else if (user.message.size() > 2 && (user.message[1][0] == '#' || user.message[1][0] == '&'))
		j = 1;
	else
	{
		sendError(user, ERR_NEEDMOREPARAMS, "KICK");
		return (1);
	}
	//std::cout << "-- KICK parameters passed " << std::endl;
	std::string channelName = user.message[j].data();
	//check that channel exist
	std::string kickUserNick = user.message[j + 1].data();
	if (kickUserNick.back() == ',')
		kickUserNick.pop_back();
	User &kickUser = *find_user(kickUserNick, _users);
	if (_channels.find(channelName)  == _channels.end())
	{
		sendError(user, ERR_NOSUCHCHANNEL, channelName);
		return (0);
	}
	Channel *ch = _channels.find(channelName)->second;
	//strange error
	if (find_user(user.getNick(), ch->getRegUsers()) == 0)
	{
		sendError(user, ERR_NOTONCHANNEL, channelName);
		return (0);
	}
	if (find_user(kickUserNick, ch->getRegUsers()) == 0)
	{
		sendError(user, ERR_USERNOTINCHANNEL, kickUserNick);
		return (0);
	}
	if (find_user(user.getNick(), ch->getOperators()) == 0)
	{	
		sendError(user, ERR_CHANOPRIVSNEEDED, channelName);
		return (0);
	}
	Channel    *chan = _channels.at(user.adressee);
	std::string us = user.message[2];
	if (us.back() == ',')
		us.pop_back();
	//us = us.substr(0, user.message[2].size()-1);
	User *adresant = find_user(us, chan->getRegUsers());
	std::cout << "-- KICK: channel exist, user found" << &kickUser << std::endl;
	if (user.text.back() == ',')
		user.text.pop_back();
	std::string    message = "KICK " + chan->getName() + " " + adresant->nickname + " :";
// if (user.message.size() > 3)
//    message += user.message[3];
// else
	message += user.nickname;
	chan->sendMessage_chan(message, user);

	//std::string mess = ":" + user.getNick() + "!" + user.user_name + "@" + user.host_name + " ";
	//ch->sendMessageToAll(*this, mess + user.text + " :" + user.getNick());
	int if1 = ch->removeOperator(kickUser);
	std::cout << "-- KICK: after removeOperator(kickUser): " << if1 << std::endl;
	std::map<std::string, Channel *>::iterator  it = _channels.find(channelName);
	if (if1 < 0)
	{
		std::cout << "exec removeOperator -- remove channel: " << ch->getName() <<std::endl;
		delete (ch);
		_channels.erase(it);
		std::cout << "-- channel removed " << std::endl;
		return (0);
	}
	//std::string mess = ":" + kickUser.getNick() + "!" + kickUser.user_name + "@" + kickUser.host_name + " ";
	//ch->sendMessageToAll(*this, mess + "PART " + channelName);
	//ch->sendMessage_chan(user.text, user);
	int if2 = ch->removeRegUser(kickUser, 0);
	if (if2 < 0 && if1 >= 0)
	{
		std::cout << "exec removeRegUser -- remove channel: " << ch->getName() << std::endl;
		delete (it->second);
		_channels.erase(it);
		std::cout << "-- channel removed " << std::endl;
		return (0);
	}
	std::cout << "-- KICK: after removeRegUser(kickUser) " << if2 << std::endl;
	//std::string mess = ":" + user.getNick() + "!" + user.user_name + "@" + user.host_name + " ";
	//ch->sendMessageToAll(*this, mess + user.text);
	return (0);
}

void	Server::bot(User &user)
{
	std::string replyStr = "";
	std::string	format = ":bot!botsrv@127.0.0.1 PRIVMSG " + user.getNick() + " ";
	if (user.message.size() < 3 )
	{
		std::cout << "Bot received empty message" << std::endl;
		replyStr = "I can Upper chars in you message. Please send me something";
	}
	else
	{
		int i = 0;
		while (user.text[i] == ' ' && i < (int)user.text.size())
			i++;
		while (user.text[i] != ' ' && i < (int)user.text.size())
			i++;
		while (user.text[i] == ' ' && i < (int)user.text.size())
			i++;
		while (user.text[i] != ' ' && i < (int)user.text.size())
			i++;
		while (user.text[i] == ' ' && i < (int)user.text.size())
			i++;	
		while (i < (int)user.text.size())
		{
			replyStr.push_back(user.text[i]);
			i++;
		}
		std::transform(replyStr.begin(), replyStr.end(), replyStr.begin(), ::toupper);
	}
	if (replyStr[0] != ':')
		replyStr.insert(0 ,":");
	replyStr = format + replyStr;

	std::cout << "Bot reply: " << replyStr.data() << std::endl;
	//sendMessage(user.getFD(), replyStr);
	user.sendMessage(replyStr);
	//replyStr = ":bot!botsrv@127.0.0.1 JOIN #ch";
	//sendMessage(user.getFD(), replyStr);
}

// ---------------------------errors ---------------------------------------

int		sendError(User &user, int num, const std::string &arg1,
					 const std::string &arg2)
{
	std::string	format = ":" + user.server_name + " " + i_to_str(num) + " " +
							user.getNick() + " "; //  ":SERVER_NAME ";

	if (num == ERR_NOSUCHNICK)
		format += arg1 + " :No such nick/channel\n";
	else if (num == ERR_NOSUCHSERVER)
		format += arg1 + " :No such server\n";
	else if (num == ERR_NOSUCHCHANNEL)
		format += arg1 + " :No such channel\n";
	else if (num == ERR_CANNOTSENDTOCHAN)
		format += arg1 + " :Cannot send to channel\n";
	else if (num == ERR_TOOMANYCHANNELS)
		format += arg1 + " :You have joined too many channels\n";
	else if (num == ERR_WASNOSUCHNICK)
		format += arg1 + " :There was no such nickname\n";
	else if (num == ERR_TOOMANYTARGETS)
		format += arg1 + " :Duplicate recipients. No arg1 delivered\n";
	else if (num == ERR_NOORIGIN)
		format += ":No origin specified\n";
	else if (num == ERR_NORECIPIENT)
		format += ":No recipient given (" + arg1 + ")\n";
	else if (num == ERR_NOTEXTTOSEND)
		format += ":No text to send\n";
	else if (num == ERR_NOTOPLEVEL)
		format += arg1 + " :No toplevel domain specified\n";
	else if (num == ERR_WILDTOPLEVEL)
		format += arg1 + " :Wildcard in toplevel domain\n";
	else if (num == ERR_UNKNOWNCOMMAND)
		format += arg1 + " :Unknown command\n";
	else if (num == ERR_NOMOTD)
		format += ":MOTD File is missing\n";
	else if (num == ERR_NOADMININFO)
		format += arg1 + " :No administrative info available\n";
	else if (num == ERR_FILEERROR)
		format += ":File error doing \n" + arg1 + " on " + arg2 + "\n";
	else if (num == ERR_NONICKNAMEGIVEN)
		format += ":No nickname given\n";
	else if (num == ERR_ERRONEUSNICKNAME)
		format += arg1 + " :Erroneus nickname\n";
	else if (num == ERR_NICKNAMEINUSE)
		format += arg1 + " :Nickname is already in use\n";
	else if (num == ERR_NICKCOLLISION)
		format += arg1 + " :Nickname collision KILL\n";
	else if (num == ERR_USERNOTINCHANNEL)
		format += arg1 + " " + arg2 + " :They aren't on that channel\n";
	else if (num == ERR_NOTONCHANNEL)
		format += arg1 + " :You're not on that channel\n";
	else if (num == ERR_USERONCHANNEL)
		format += arg1 + " " + arg2 + " :is already on channel\n";
	else if (num == ERR_NOLOGIN)
		format += arg1 + " :User not logged in\n";
	else if (num == ERR_SUMMONDISABLED)
		format += ":SUMMON has been disabled\n";
	else if (num == ERR_USERSDISABLED)
		format += ":USERS has been disabled\n";
	else if (num == ERR_NOTREGISTERED)
		format += ":You have not registered\n";
	else if (num == ERR_NEEDMOREPARAMS)
		format += arg1 + " :Not enough parameters\n";
	else if (num == ERR_ALREADYREGISTRED)
		format += ":You may not reregister\n";
	else if (num == ERR_NOPERMFORHOST)
		format += ":Your host isn't among the privileged\n";
	else if (num == ERR_PASSWDMISMATCH)
		format += ":Password incorrect\n";
	else if (num == ERR_YOUREBANNEDCREEP)
		format += ":You are banned from this server\n";
	else if (num == ERR_KEYSET)
		format += arg1 + " :Channel key already set\n";
	else if (num == ERR_CHANNELISFULL)
		format += arg1 + " :Cannot join channel (+l)\n";
	else if (num == ERR_UNKNOWNMODE)
		format += arg1 + " :is unknown mode char to me\n";
	else if (num == ERR_INVITEONLYCHAN)
		format += arg1 + " :Cannot join channel (+i)\n";
	else if (num == ERR_BANNEDFROMCHAN)
		format += arg1 + " :Cannot join channel (+b)\n";
	else if (num == ERR_BADCHANNELKEY)
		format += arg1 + " :Cannot join channel (+k)\n";
	else if (num == ERR_NOPRIVILEGES)
		format += ":Permission Denied- You're not an IRC operator\n";
	else if (num == ERR_CHANOPRIVSNEEDED)
		format += arg1 + " :You're not channel operator\n";
	else if (num == ERR_CANTKILLSERVER)
		format += ":You cant kill a server!\n";
	else if (num == ERR_NOOPERHOST)
		format += ":No O-lines for your host\n";
	else if (num == ERR_UMODEUNKNOWNFLAG)
		format += ":Unknown MODE flag\n";
	else if (num == ERR_USERSDONTMATCH)
		format += ":Cant change mode for other users\n";
	else
		format += "UNKNOWN ERROR\n";
	send(user.getFD(), format.c_str(), format.size(), IRC_NOSIGNAL);
	return (-1);
}

int		sendReply(User &user, int num, const std::string &arg1, const
			std::string &arg2, const std::string &arg3, const std::string &arg4)
{
	std::string	format = ":" + user.server_name + " " + i_to_str(num) + " " + user
			.nickname + " ";

	if (num == RPL_USERHOST)
		format += ":" + arg1 + "\n";
	else if (num == RPL_ISON)
		format += ":" + arg1 + "\n";
	else if (num == RPL_AWAY)
		format += arg1 + " :" + arg2 + "\n";
	else if (num == RPL_UNAWAY)
		format += ":You are no longer marked as being away\n";
	else if (num == RPL_NOWAWAY)
		format += ":You have been marked as being away\n";
	else if (num == RPL_WHOISUSER)
		format += arg1 + " " + arg2 + " " + arg3 + " * :" + arg4 + "\n";
	else if (num == RPL_WHOISSERVER)
		format += arg1 + " " + arg2 + " :" + arg3 + "\n";
	else if (num == RPL_WHOISOPERATOR)
		format += arg1 + " :is an IRC operator\n";
	else if (num == RPL_WHOISIDLE)
		format += arg1 + " " + arg2 + " " + arg3 + " :seconds idle\n";
	else if (num == RPL_ENDOFWHOIS)
		format += arg1 + " :End of /WHOIS list\n";
	else if (num == RPL_WHOISCHANNELS)
		format += arg1 + " :" + arg2 + "\n";
	else if (num == RPL_WHOWASUSER)
		format += arg1 + " " + arg2 + " " + arg3 + " * :" + arg4 + "\n";
	else if (num == RPL_ENDOFWHOWAS)
		format += arg1 + " :End of WHOWAS\n";
	else if (num == RPL_LISTSTART)
		format += "Channel :Users  Name\n";
	else if (num == RPL_LIST)
		format += arg1 + " " + arg2 + " :" + arg3 + "\n";
	else if (num == RPL_LISTEND)
		format += ":End of /LIST\n";
	else if (num == RPL_CHANNELMODEIS)
		format += arg1 + " +" + arg2 + "\n";
	else if (num == RPL_NOTOPIC)
		format += arg1 + " :No topic is set\n";
	else if (num == RPL_TOPIC)
		format += arg1 + " :" + arg2 + "\n";
	else if (num == RPL_INVITING)
		format += arg1 + " " + arg2 + "\n";
	else if (num == RPL_SUMMONING)
		format += arg1 + " :Summoning user to IRC\n";
	else if (num == RPL_VERSION)
		format += arg1 + "." + arg2 + " " + arg3 + " :" + arg4 + "\n";
	else if (num == RPL_WHOREPLY)
		format += arg1 + "\n";
	else if (num == RPL_ENDOFWHO)
		format += arg1 + " :End of /WHO list\n";
	else if (num == RPL_NAMREPLY)
		format += arg1 + " :" + arg2 + "\n";
	else if (num == RPL_ENDOFNAMES)
		format += arg1 + " :End of /NAMES list\n";
	else if (num == RPL_LINKS)
		format += arg1 + " " + arg2 + ": " + arg3 + " " + arg4 + "\n";
	else if (num == RPL_ENDOFLINKS)
		format += arg1 + " :End of /LINKS list\n";
	else if (num == RPL_BANLIST)
		format += arg1 + " " + arg2 + "\n";
	else if (num == RPL_ENDOFBANLIST)
		format += arg1 + " :End of channel ban list\n";
	else if (num == RPL_INFO)
		format += ":" + arg1 + "\n";
	else if (num == RPL_ENDOFINFO)
		format += ":End of /INFO list\n";
	else if (num == RPL_MOTDSTART)
		format += ":- " + arg1 + " Message of the day - \n";
	else if (num == RPL_MOTD)
		format += ":- " + arg1 + "\n";
	else if (num == RPL_ENDOFMOTD)
		format += ":End of /MOTD command\n";
	else if (num == RPL_YOUREOPER)
		format += ":You are now an IRC operator\n";
	else if (num == RPL_REHASHING)
		format += arg1 + " :Rehashing\n";
	else if (num == RPL_TIME)
		format += arg1 + " :" + arg2;
	else if (num == RPL_USERSSTART)
		format += ":UserID   Terminal  Host\n";
	else if (num == RPL_USERS)
		format += ":%-8s %-9s %-8s\n";
	else if (num == RPL_ENDOFUSERS)
		format += ":End of users\n";
	else if (num == RPL_NOUSERS)
		format += ":Nobody logged in\n";
	else if (num == RPL_TRACELINK)
		format += "Link " + arg1 + " " + arg2 + " " + arg3 + "\n";
	else if (num == RPL_TRACECONNECTING)
		format += "Try. " + arg1 + " " + arg2 + "\n";
	else if (num == RPL_TRACEHANDSHAKE)
		format += "H.S. " + arg1 + " " + arg2 + "\n";
	else if (num == RPL_TRACEUNKNOWN)
		format += "???? " + arg1 + " " + arg2 + "\n";
	else if (num == RPL_TRACEOPERATOR)
		format += "Oper " + arg1 + " " + arg2 + "\n";
	else if (num == RPL_TRACEUSER)
		format += "User " + arg1 + " " + arg2 + "\n";
	else if (num == RPL_TRACENEWTYPE)
		format += arg1 + " 0 " + arg2 + "\n";
	else if (num == RPL_TRACELOG)
		format += "File " + arg1 + " " + arg2 + "\n";
	else if (num == RPL_STATSCOMMANDS)
		format += arg1 + " " + arg2 + "\n";
	else if (num == RPL_STATSCLINE)
		format += "C " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
	else if (num == RPL_STATSNLINE)
		format += "N " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
	else if (num == RPL_STATSILINE)
		format += "I " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
	else if (num == RPL_STATSKLINE)
		format += "K " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
	else if (num == RPL_STATSYLINE)
		format += "Y " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
	else if (num == RPL_ENDOFSTATS)
		format += arg1 + " :End of /STATS report\n";
	else if (num == RPL_STATSLLINE)
		format += "L " + arg1 + " * " + arg2 + " " + arg3 + "\n";
	else if (num == RPL_STATSUPTIME)
		format += ":Server Up %d days %d:%02d:%02d\n";
	else if (num == RPL_STATSOLINE)
		format += "O " + arg1 + " * " + arg2 + "\n";
	else if (num == RPL_STATSHLINE)
		format += "H " + arg1 + " * " + arg2 + "\n";
	else if (num == RPL_UMODEIS)
		format += arg1 + "\n";
	else if (num == RPL_LUSERCLIENT)
		format += ":There are " + arg1 + " users and " + arg2 + " invisible on "
				  + arg3 + " servers\n";
	else if (num == RPL_LUSEROP)
		format += arg1 + " :operator(s) online\n";
	else if (num == RPL_LUSERUNKNOWN)
		format += arg1 + " :unknown connection(s)\n";
	else if (num == RPL_LUSERCHANNELS)
		format += arg1 + " :channels formed\n";
	else if (num == RPL_LUSERME)
		format += ":I have " + arg1 + " clients and " + arg2 + " servers\n";
	else if (num == RPL_ADMINME)
		format += arg1 + " :Administrative info\n";
	else if (num == RPL_ADMINLOC1)
		format += ":Name     " + arg1 + "\n";
	else if (num == RPL_ADMINLOC2)
		format += ":Nickname " + arg1 + "\n";
	else if (num == RPL_ADMINEMAIL)
		format += ":E-Mail   " + arg1 + "\n";
	else
		format += "UNKNOWN REPLY\n";
	send(user.getFD(), format.c_str(), format.size(), IRC_NOSIGNAL);
	return 0;
}

std::string	i_to_str(size_t number)
{
	std::ostringstream ss;
	ss << number;
	return (ss.str());
}