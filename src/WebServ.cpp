/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 22:02:58 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/07/19 10:24:54 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/WebServ.hpp"
#include "../include/Core.hpp"
#include "../include/Tokenization.hpp"

bool	webserv(const char *av)
{
	std::vector<Server> servers;
	std::string s(av);
	std::ifstream file(s);
	if (not file.is_open() or not parcing(file, s))
		throw std::runtime_error("Error");

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

	return (true);
}