/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dtawney <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/07 16:45:35 by dtawney           #+#    #+#             */
/*   Updated: 2021/11/07 16:45:38 by dtawney          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>

//class Server;
#include "Server.hpp"

int		check_port(char *str)
{
	int		port_num = 0;
	port_num = std::atoi(str);
	if (port_num < 102 || port_num > 64000)
	{
		std::cout << "Provided port number is not acceptable" << std::endl;
		std::cout << "Notice: Default port for IRC servers is usualy 6667." << std::endl;
		exit (0);
	}
	return (port_num);
}

int	main(int argc, char **argv)
{
	if (argc < 3 || argc > 4)
	{
		std::cout << "## start should be like:" << std::endl;
		std::cout << "./ircserv [host:port_network:password_network] <port> <password>" << std::endl;
		std::cout << "## but server-server communication is out of this project" << std::endl;
		return (0);
	}
	if (argc == 3)
	{
		Server ircserv(check_port(argv[1]), argv[2]);
		ircserv.start();
	}
	if (argc == 4)
	{
		Server ircserv(check_port(argv[2]), argv[3]);
		ircserv.start();
	}
	return 0;
}