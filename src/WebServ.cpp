/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 22:02:58 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/06/22 17:27:05 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include "Core.hpp"
#include "Tokenization.hpp"

bool	webserv(const char *av)
{
	std::vector<Server> servers;
	std::string s(av);
	std::ifstream file(s);
	if (not file.is_open() or not parcing(file, s))
		throw std::runtime_error("Error");

	file.open(s.c_str());
	servers = Tokenization(file);

	signal(SIGPIPE, SIG_IGN);
	Core core( servers );
	core.start();

	file.close();

	return (true);
}