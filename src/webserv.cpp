/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 22:02:58 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/05/23 10:51:01 by aaggoujj         ###   ########.fr       */
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

void	setRequest(std::map<int, Request> &requests,const std::vector<struct pollfd> &pollfds)
{
	for (size_t i = 0; i < pollfds.size(); i++)
	{
		if (pollfds[i].fd != -1)
		{
			Request request;
			requests[pollfds[i].fd] = request;
		}
	}
}

void	Polloop(Core &core)
{
	int ret;
	bool isDone;
	std::map<int, Request> requests;

	while ( 1337 )
	{
		ret = poll(const_cast<struct pollfd *>(core.get_pollfds().data()), core.get_pollfds().size(), -1);
		if (ret == -1)
			throw std::runtime_error("poll() failed");
		for (size_t i = 0; i < core.get_pollfds().size(); i++)
		{
			if (core.get_pollfds()[i].revents == 0)
				continue ;
			if (core.get_pollfds()[i].revents & POLLIN)
			{
				if (core.get_pollFdsSet().find(core.get_pollfds()[i].fd) != core.get_pollFdsSet().end())
					core.acceptConnection(requests);
				else
				{
					isDone = core.readRequest(core.get_pollfds()[i].fd, requests);
				}
			}
			if (core.get_pollfds()[i].revents & POLLOUT)
			{
				if (requests[core.get_pollfds()[i].fd].state == DONE or requests[core.get_pollfds()[i].fd].state == ERR)
					{
						std::cout << "response sent" << std::endl;
						core.sendResponse(requests[core.get_pollfds()[i].fd], core.get_pollfds()[i].fd);
						requests[core.get_pollfds()[i].fd].clear();
					}
			}
		}
	}
}

void    createSocket(Core &core)
{
	std::map<int, std::vector<Server> > socketFds = core.get_core();
	std::map<int, std::vector<Server> >::iterator it = socketFds.begin();
	struct addrinfo hints, *ai;

	core.set_pollfds();
	while (it != socketFds.end())
	{
		int socketFd = it->first;
		int opt = 1;
		if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			throw std::runtime_error("setsockopt() failed");
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		if (getaddrinfo(it->second[0].getHostListen().c_str(),  std::to_string(it->second[0].getPort()).c_str(), &hints, &ai) != 0)
			throw std::runtime_error("getaddrinfo() failed");
		if (fcntl(socketFd, F_SETFL, O_NONBLOCK) == -1)
			throw std::runtime_error("fcntl() failed");
		if (bind(socketFd, ai->ai_addr, ai->ai_addrlen) == -1)
			throw std::runtime_error("bind() failed");
		if (listen(socketFd, SOMAXCONN) == -1)
			throw std::runtime_error("listen() failed");
		freeaddrinfo(ai);
		it++;
	}
	Polloop(core);
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
	//* printing servers ************************//
	// for (size_t i = 0; i < servers.size(); i++)
	// 	print_servers(servers[i], i);

	Core core( servers );
	for (auto it : core.get_core())
	{
	    std::cout << it.first << " = ";
	    for (auto it2 : it.second)
	        std::cout << it2.getHost() << " ";
	    std::cout << std::endl;
	}
	createSocket(core);
	return (true);
}