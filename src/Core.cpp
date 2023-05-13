/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Core.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 15:20:59 by moseddik          #+#    #+#             */
/*   Updated: 2023/05/13 16:10:21 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Core.hpp"

Core::Core( std::vector<Server> servers )
{
	for (size_t i = 0; i < servers.size(); i++)
		this->addServer(servers[i]);

	this->addSocketFd();
	return ;
}

Core::~Core(void)
{
	this->_core.clear();
	this->_ports.clear();
	return ;
}

void Core::addServer(Server server)
{
	// std::cout << server.getPort() << std::endl;
	if (this->_core.find(server.getPort()) == this->_core.end())
	{
		std::vector<Server> servers;
		servers.push_back(server);
		this->_core.insert(std::pair<int, std::vector<Server> >(server.getPort(), servers));
	}
	else
		this->_core[server.getPort()].push_back(server);
}

void Core::addSocketFd( void )
{
	int socketFd;

	for ( std::map<int, std::vector<Server> >::iterator it = this->_core.begin(); it != this->_core.end(); it++)
	{
		socketFd = socket(AF_INET, SOCK_STREAM, 0);
		if (socketFd == -1)
			throw std::runtime_error("socket() failed");
		this->_socketFds[socketFd] = it->second;
	}
}

std::map<int, std::vector<Server> > const & Core::get_core(void) const
{
	return this->_socketFds;
}

bool Core::find_port(int port)
{
	return this->_ports.find(port) != this->_ports.end();
}

void Core::set_pollfds(void)
{
	this->_pollfds.clear();
	for ( std::map<int, std::vector<Server> >::iterator it = this->_socketFds.begin(); it != this->_socketFds.end(); it++)
	{
		this->_pollfds.push_back((struct pollfd) {
			.fd = it->first,
			.events = POLLIN | POLLOUT,
			.revents = 0
		});
		this->_pollFdsSet.insert(it->first);
	}
}

std::vector<struct pollfd> const & Core::get_pollfds(void) const
{
	return this->_pollfds;
}

std::set<int> const & Core::get_pollFdsSet( void ) const
{
	return this->_pollFdsSet;
}

void Core::acceptConnection(void)
{
	int newFd;
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	for ( size_t i = 0; i < this->_pollfds.size(); i++ )
	{
		if (this->_pollfds[i].revents & POLLIN)
		{
			newFd = accept(this->_pollfds[i].fd, (struct sockaddr *)&clientAddr, &clientAddrLen);
			fcntl(newFd, F_SETFL, O_NONBLOCK);
			std::cout << "Accept new connection with fd = " << newFd << std::endl;
			if (newFd == -1)
				throw std::runtime_error("accept() failed");
			this->_pollfds.push_back((struct pollfd) {
				.fd = newFd,
				.events = POLLIN | POLLOUT,
				.revents = 0
			});
		}
	}
}

std::vector<std::string> split(std::string & str, std::string const & delim)
{
	std::vector<std::string> result;
	size_t start = 0;
	size_t end = str.find(delim);

	while (end != std::string::npos)
	{
		result.push_back(str.substr(start, end - start));
		start = end + delim.length();
		end = str.find(delim, start);
	}
	result.push_back(str.substr(start, end));
	return result;
}

void	requestLine(std::string & line)
{
	(void)line;
}

void	requestHeaders(std::vector<std::string> & lines)
{
	(void)lines;
}

void	requestBody(std::vector<std::string> & lines)
{
	(void)lines;
}

void parseRequest(std::string &request)
{
	std::vector<std::string> lines = split(request, "\r\n");
	requestLine(lines[0]);
	requestHeaders(lines);
	requestBody(lines);
}

void Core::readRequest(int fd)
{
	char buffer[MAX_BUFFSIZE];
	std::string request;
	int readBytes;

	readBytes = recv(fd, buffer, MAX_BUFFSIZE, 0);
	if (readBytes == -1)
		throw std::runtime_error("recv() failed");
	else if (readBytes == 0)
	{
		close(fd);
		this->_pollFdsSet.erase(fd);
	}
	else{
		buffer[readBytes] = '\0';
		request = buffer;
		parseRequest(request);
	}
}

void Core::set_ports(int port)
{
	this->_ports.insert(port);
}
