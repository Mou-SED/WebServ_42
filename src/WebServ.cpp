/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayassir <ayassir@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 22:02:58 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/07/18 16:18:05 by ayassir          ###   ########.fr       */
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

	std::cout << "Ports available : 12345(webserv), 8080(app.py), 8070(cookies), 8060(session).\n";
	file.open(s.c_str());
	servers = Tokenization(file);

	signal(SIGPIPE, SIG_IGN);
	Core core( servers );
	core.start();

	file.close();

	return (true);
}