/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 22:02:58 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/07/19 13:24:12 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/WebServ.hpp"
#include "../include/Core.hpp"
#include "../include/Tokenization.hpp"

void	webserv(const char *av)
{
	std::vector<Server> servers;
	std::string s(av);
	std::ifstream file(s);
	if (not file.is_open() or not parcing(file, s))
		throw std::runtime_error("Error: config file not found or invalid!, please check the path");

	file.open(s.c_str());
	servers = Tokenization(file);
	std::cout << "Server Available: ";
	for (size_t i = 0; i < servers.size(); i++)
		std::cout << servers[i].getPort() << "(" << servers[i].getServerName() << ")" << ", ";
	std::cout << std::endl;
	signal(SIGPIPE, SIG_IGN);
	Core core( servers );
	core.start();

	file.close();

	return ;
}