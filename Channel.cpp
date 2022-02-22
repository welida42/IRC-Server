#include "Channel.hpp"

Channel::Channel(std::string const &name)
{
	this->_name = name;
	_topic = "";
}
	
Channel::~Channel() {}

std::string		Channel::getName() 
{
	return (this->_name);
}

int				Channel::addRegUser(User &user)
{
	//need to check if it alredy in array
	_regularUsers.push_back(&user);
	std::cout << "joined to channel " << this->_name << " as regular user: " << user.getNick() << std::endl;
	return (0);
}

int				Channel::addOperator(User &user)
{
	//need to check if it alredy in array
	_operators.push_back(&user);
	std::cout << "joined to channel " << this->_name << " as operator: " << user.getNick() << std::endl;
	return (0);
}

int				Channel::removeOperator(User &user)
{
	// if after user removing nobody are exist in the Operators the first regUser should become a new Operator
	// return 0 if the user was not found in Channel Operators list
	// return 1 if just a user was removed from Channel Operators list
	// return 2 if removed user was last user and new Channel Operator promoted
	// return 3 if users are not exist (empty Channel Operators list), but a new Operator promoted
	// return -1 if users ane not exist (empty Channel Operators list) and a new Operator can't be promoted because they are not exist 

	int		i_remove = 0;
	std::string format;
	format = ":" + user.nickname + "!" + user.user_name + "@" + user.host_name
			 + " MODE " + this->getName() + " -o " + user.getNick();
	std::vector<User *>::iterator  it = _operators.begin();
	std::vector<User *>::iterator  ite = _operators.end();
	if (_operators.empty())
		std::cout << "Operators list in Channel " <<  this->getName() << " is empty before removing."<<std::endl;
	else
	{
		for (; it != ite; it++ )
		{
			if ((*it) == &user)
			{
				std::vector<User *>::iterator it_rg = _regularUsers.begin();
				std::vector<User *>::iterator end_rg = _regularUsers.end();
				for (; it_rg != end_rg; ++it_rg)
					(*it_rg)->sendMessage(format);
				_operators.erase(it);
				i_remove++;
			}
		}
	}
	if (i_remove == 0)
	{
		std::cout << "Operators list in Channel " <<  this->getName() << " did not contain a user " << user.getNick() <<std::endl;
		return (0);
	}
	if (i_remove == 1 && _operators.empty() && _regularUsers.size() == 1)
	{
		std::cout << "No one can be promoted" << std::endl;
		return (-1);
	}
	if (_operators.empty())
	{
		std::cout << "Operators list in Channel " <<  this->getName() << " is empty after removing. Need to add a new Operator."<<std::endl;
		if (_regularUsers.empty())
		{
			std::cout << "Regular Users are not found in the Channel " <<  this->getName() << " , it should be removed."<<std::endl;
			return (-1);
		} 
		else
		{
			// check that first regular User != provided User
			if (i_remove)
			{
				std::vector<User *>::iterator  it_rg = _regularUsers.begin();
				std::vector<User *>::iterator  ite_rg = _regularUsers.end();
				for (; it_rg != ite_rg; it_rg++)
				{
					if (*it_rg == &user)
						continue;
					else
					{
						_operators.push_back(*it_rg);
						std::cout << "Last operator was removed from the Channel " 
						<<  this->getName()
						<< " , a user with nickname "
						<< (*it_rg)->getNick() 
						<< " has been promoted to the Channel Operator." 
						<< std::endl;
						format = ":" + user.nickname + "!" + user.user_name + "@" + user.host_name
			 					+ " MODE " + this->getName() + " +o " + (*it_rg)->getNick();
						std::string format2;
						format2 = ":" + user.getNick() + "!" + user.user_name + "@" + user.host_name
			 					+ " PART ";
						std::vector<User *>::iterator it_rg2 = _regularUsers.begin();
						std::vector<User *>::iterator end_rg2 = _regularUsers.end();
						for (; it_rg2 != end_rg2; ++it_rg2)
						{
							(*it_rg2)->sendMessage(format);
							//(*it_rg2)->sendMessage_chan(format2 + this->getName());
						}
						break;
					}
				}
				
				return (2);
			}
			std::cout << "No one operator was removed from the Channel "
						<<  this->getName()
						<< " , a user with nickname "
						<< (*_regularUsers.begin())->getNick() 
						<< " has been promoted to the Channel Operator." 
						<< std::endl;
			return (3);
		}
	}
	else
	{
		std::cout << "Operators list in Channel " <<  this->getName() << " was cleaned from user " << user.getNick() <<std::endl;
		return (1);
	}
	return (0);
}

int				Channel::removeRegUser(User &user, int fl_quit)
{
	// if users are not exist in the channel it should be removed
	// return 0 if the user was not found in Channel list
	// return 1 if just a user was removed
	// return -1 if removed user was last user and the channel should be removed
	// return -2 if users ane not exist (empty channel users) and the channel should be removed
	if (_regularUsers.empty())
		return (-2);
	int i_remove = 0;
	std::string format2;
	std::vector<User *>::iterator  it_rg = _regularUsers.begin();
	std::vector<User *>::iterator  ite_rg = _regularUsers.end();
	for (; it_rg != ite_rg; it_rg++ )
	{
		if ((*it_rg) == &user)
		{
			format2 = ":" + (*it_rg)->getNick() + "!" + (*it_rg)->user_name + "@" + (*it_rg)->host_name
					+ " PART " + this->getName();
			_regularUsers.erase(it_rg);
			if (fl_quit)
			{
				std::vector<User *>::iterator it_rg2 = _regularUsers.begin();
				std::vector<User *>::iterator end_rg2 = _regularUsers.end();
				for (; it_rg2 != end_rg2; ++it_rg2)
					(*it_rg2)->sendMessage(format2);
			}
			i_remove++;
		}
	}
	if (!i_remove)
		return (0);
	if (_regularUsers.empty())
		return (-1);
	return (1);
}

int				Channel::sendMessageToAll(Server &server, std::string const &str)
{
	std::string message = str;
	std::vector<User *>::iterator  it_rg = _regularUsers.begin();
	std::vector<User *>::iterator  ite_rg = _regularUsers.end();
	for (; it_rg != ite_rg; it_rg++ )
		server.sendMessage((*it_rg)->getFD(), message);
	return (0);
}

std::string		Channel::getUsersList()
{
	std::string str;
	int		flag = 0;
	std::vector<User *>::iterator  it = _operators.begin();
	std::vector<User *>::iterator  ite = _operators.end();
	for (; it != ite; it++ )
		str = str + "@" + (*it)->getNick() + " ";
	std::vector<User *>::iterator  it_rg = _regularUsers.begin();
	std::vector<User *>::iterator  ite_rg = _regularUsers.end();
	for (; it_rg != ite_rg; it_rg++)
	{
		it = _operators.begin();
		flag = 0;
		for (; it != ite; it++ )
			if ((*it)->getNick() == (*it_rg)->getNick())
			{
				flag = 1;
				break;
			}
		if (!flag)
			str = str + (*it_rg)->getNick() + " ";
	}
	return (str);
}