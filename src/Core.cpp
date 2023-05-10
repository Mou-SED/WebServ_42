/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Core.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 15:20:59 by moseddik          #+#    #+#             */
/*   Updated: 2023/05/10 20:39:20 by aaggoujj         ###   ########.fr       */
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

// for (i = 0; i < num_clients; i++) {
//             if (fds[i + 1].revents & POLLIN) {
//                 // Read data from client socket
//                 char buffer[1024];
//                 int num_bytes = recv(client_socket[i], buffer, sizeof(buffer), 0);
//                 if (num_bytes == -1) {
//                     perror("recv");
//                     return EXIT_FAILURE;
//                 } else if (num_bytes == 0) {
//                     // Connection closed by client
//                     close(client_socket[i]);
//                     fds[i + 1].fd = -1;
//                     num_clients--;
//                     printf("Client disconnected\n");
//                 } else {
//                     // Echo data back to client
//                     if (send(client_socket[i], buffer, num_bytes, 0) == -1) {
//                         perror("send");
//                         return EXIT_FAILURE;
//                     }
//                 }
//             }
// }

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

void Core::readRequest(int fd)
{
	char buffer[1024];
	int readBytes;

	std::cout << fd << std::endl;
	readBytes = recv(fd, buffer, 1024, 0);
	if (readBytes == -1)
		throw std::runtime_error("recv() failed");
	else if (readBytes == 0)
	{
		close(fd);
		this->_pollFdsSet.erase(fd);
	}
	else
	{
		buffer[readBytes] = '\0';
		std::cout << buffer << std::endl;
	}
}

void Core::set_ports(int port)
{
	this->_ports.insert(port);
}
