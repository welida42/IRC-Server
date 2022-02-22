#include "User.hpp"

User::User(int fd, struct sockaddr_in client, std::string &server_name)
{
	this->_fd = fd;
	this->client = client;
	this->host_name = inet_ntoa(client.sin_addr); //.s_addr;
	this->registered = false;
	this->pass_ok = false;
	this->_away = false;
	this->nickname = "";
	this->adressee = "";
	this->cmd_string = "";
	this->command = "";
	this->server_name = server_name;
	this->away_text = "";
	this->server_operator = false;
	std::cout << "Client with fd=" << this->_fd << " with hostname = " << host_name <<
		" is added"	<< std::endl;
}

User::~User()
{
	std::cout << "Client with fd=" << this->_fd << " is removed" << std::endl;
}

User&::User::operator=( const User &rhs )
{
	if (this == &rhs)
		return *this;
	this->_fd = rhs._fd;
	this->client = rhs.client;
	return *this;
}

void	User::printUser()
{
	std::cout << "User with fd=" << this->_fd << " exist in list" << std::endl;
}

int		User::getFD()
{
	return (this->_fd);
}

std::string		User::getNick()
{
	return (this->nickname);
}

void		User::setNick( std::string const &nickname )
{
	this->nickname = nickname;
}

int		User::splitToMessage()
{
	int		j = 0;
	int		i = 0;

	if (text.length() > 0)
	{
		//std::cout << "## splitting start" << std::endl;
		while (i < (int)text.length())
		{
			while (text[i] == ' ')
				i++;
			//std::cout << "## splitting i = " << i << ", j = " << j << std::endl;
			j = text.find(' ', i);
			if (j > i)
				message.push_back(text.substr(i, j - i));
			else
			{
				message.push_back(text.substr(i, text.length()));
				break;
			}
			i = j;
			i++;
		}
	for (int p = 0; p < (int)message.size(); p++)
		std::cout << "Message part #" << p << " has text: '" << message[p] << "'" << std::endl;
	if (message[0][0] == ':')
			message.erase(message.begin());
	for (int p = 0; p < (int)message.size(); p++)
	{
		if (p == 0)
			this->command = message[p];
		if (p == 1)
			this->adressee = message[p];
		if (p == 2)
			this->cmd_string = message[p];
		if (p >= 3)
		{
			this->cmd_string += " ";
			this->cmd_string += message[p];
		}
	}
		// need to convert a command to uppercase, remove : from sender
		return (0);
	}
	return (-1);
}