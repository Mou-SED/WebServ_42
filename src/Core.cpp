/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Core.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 15:20:59 by moseddik          #+#    #+#             */
/*   Updated: 2023/05/21 16:47:09 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Core.hpp"

Core::Core(std::vector<Server> servers)
{
	for (size_t i = 0; i < servers.size(); i++)
		this->addServer(servers[i]);

	this->addSocketFd();
	return;
}

Core::~Core(void)
{
	this->_core.clear();
	this->_ports.clear();
	return;
}

void Core::addServer(Server server)
{
	if (this->_core.find(server.getPort()) == this->_core.end())
	{
		std::vector<Server> servers;
		servers.push_back(server);
		this->_core.insert(std::pair<int, std::vector<Server>>(server.getPort(), servers));
	}
	else
		this->_core[server.getPort()].push_back(server);
}

void Core::addSocketFd(void)
{
	int socketFd;

	for (std::map<int, std::vector<Server>>::iterator it = this->_core.begin(); it != this->_core.end(); it++)
	{
		socketFd = socket(AF_INET, SOCK_STREAM, 0);
		if (socketFd == -1)
			throw std::runtime_error("socket() failed");
		this->_socketFds[socketFd] = it->second;
	}
}

std::map<int, std::vector<Server>> const &Core::get_core(void) const
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
	for (std::map<int, std::vector<Server>>::iterator it = this->_socketFds.begin(); it != this->_socketFds.end(); it++)
	{
		this->_pollfds.push_back((struct pollfd){
			.fd = it->first,
			.events = POLLIN | POLLOUT,
			.revents = 0});
		this->_pollFdsSet.insert(it->first);
	}
}

std::vector<struct pollfd> const &Core::get_pollfds(void) const
{
	return this->_pollfds;
}

std::set<int> const &Core::get_pollFdsSet(void) const
{
	return this->_pollFdsSet;
}

void Core::removeRequest(int fd, std::map<int, Request> &Requests)
{
	std::cout << "Remove request with fd = " << fd << std::endl;
	Requests[fd].clear();
	Requests.erase(fd);
	std::cout << "Request size = " << Requests.size() << std::endl;
}

void Core::acceptConnection(std::map<int, Request> &Requests)
{
	int newFd;
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	for (size_t i = 0; i < this->_pollfds.size(); i++)
	{
		if (this->_pollfds[i].revents & POLLIN)
		{
			newFd = accept(this->_pollfds[i].fd, (struct sockaddr *)&clientAddr, &clientAddrLen);
			fcntl(newFd, F_SETFL, O_NONBLOCK);
			std::cout << "Accept new connection with fd = " << newFd << std::endl;
			if (newFd == -1)
				throw std::runtime_error("accept() failed");
			this->_pollfds.push_back((struct pollfd){
				.fd = newFd,
				.events = POLLIN | POLLOUT,
				.revents = 0});
			Requests[newFd] = Request();
		}
	}
}

std::vector<std::string> split(std::string &str, char delim, bool keepIt)
{
	std::vector<std::string> result;
	size_t start = 0;

	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == delim)
		{
			if (keepIt)
				result.push_back(str.substr(start, i - start + 1));
			else
				result.push_back(str.substr(start, i - start));
			start = i + 1;
		}
	}
	if (start < str.size())
		result.push_back(str.substr(start));
	return result;
}

bool Core::readRequest(int fd, std::map<int, Request> &req)
{
	char buffer[MAX_BUFFSIZE];
	int readBytes;

	bzero(buffer, MAX_BUFFSIZE);
	readBytes = recv(fd, buffer, MAX_BUFFSIZE, 0);
	if (readBytes == -1)
		return (req[fd].status = INTERNAL_SERVER_ERROR, true);
	else if (readBytes == 0)
	{
		std::cout << "Connection closed with fd = " << fd << std::endl;
		removeRequest(fd, req);
		this->_pollFdsSet.erase(fd);
		close(fd);
	}
	else
	{
		return req[fd].mainRequest(buffer, readBytes);
	}
	return false;
}

void Core::sendResponse(Request &req, int fd)
{
	std::cout << "!!!!!!!!!!! sending to fd = " << fd << std::endl;
	std::cout << "!!!!!!!!!!! status = " << req.status << std::endl;
	if (req.status == 400)
		send(fd, "HTTP/1.1 400 Bad Request\r\n", 26, 0);
	else if (req.status == 500)
		send(fd, "HTTP/1.1 500 Internal Server Error\r\n", 36, 0);
	else if (req.status == 200)
	{

		std::string response = "HTTP/1.1 200 OK\r\n"
							   "Content-Type: text/plain\r\n"
							   "Content-Length: 12\r\n"
							   "\r\n"
							   "Hello world!";
		send(fd, response.c_str(), response.length(), 0);
	}
	return;
}

void Core::set_ports(int port)
{
	this->_ports.insert(port);
}
