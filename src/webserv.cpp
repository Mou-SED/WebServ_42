/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 22:02:58 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/06/09 17:20:29 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include "Core.hpp"
#include "Tokenization.hpp"

void	print_servers(Server &servers, int ind)
{
	std::cout << "Sever block" << std::endl;
	for (int i = 0; i < ind; i++)
		std::cout << "---";
	std::cout << "Directives:" << std::endl;
	std::cout << servers.directives.size() << std::endl;
	for (const auto& [key, values] : servers.directives)
	{
		std::cout << "- " << key << ": ";
		for (const auto& value : values)
		{
			std::cout << value << " ";
		}
		std::cout << std::endl;
	}

	std::cout << "Contexts:" << std::endl;
	for (const auto& [name, context] : servers.context)
	{
		std::cout << "- " << name << ":" << std::endl;
		for (const auto& [key, values] : context)
		{
			std::cout << "  - " << key << ": ";
			for (const auto& value : values)
			{
				std::cout << value << " ";
			}
			std::cout << std::endl;
		}
	}

}

bool	webserv(const char *av)
{
	std::vector<Server> servers;
	std::string s(av);
	std::ifstream file(s);

	if (not file.is_open() or not parcing(file, s))
		throw std::runtime_error("Error");

	file.open(s.c_str());
	servers = Tokenization(file);

	Core core( servers );
	core.start();

	file.close();

	return (true);
}