/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Core.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 15:20:59 by moseddik          #+#    #+#             */
/*   Updated: 2023/06/09 14:52:36 by aaggoujj         ###   ########.fr       */
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

std::map<int, Response> const &Core::get_responses(void) const
{
	return this->responses;
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

void Core::cleanResponse(int fd)
{
	this->responses[fd].clear();
	this->responses.erase(fd);
}

void Core::removeRequest(int fd, std::map<int, Request> &Requests)
{
	Requests[fd].clear();
	Requests.erase(fd);
}

void Core::acceptConnection(const struct pollfd &pfd, int serverFd)
{
	int newFd;
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);

	newFd = accept(pfd.fd, (struct sockaddr *)&clientAddr, &clientAddrLen);

	fcntl(newFd, F_SETFL, O_NONBLOCK);
	std::cout << "Accept new connection with fd = " << newFd << std::endl;
	if (newFd == -1)
		throw std::runtime_error("accept() failed" + std::string(strerror(errno)));

	this->_pollfds.push_back((struct pollfd){
		.fd = newFd,
		.events = POLLIN | POLLOUT,
		.revents = 0});

	this->_socketFds[newFd] = this->_socketFds[serverFd];

	this->requests[newFd] = Request();
	this->responses[newFd] = Response();
}

Server &Core::getServer(int fd, const std::string & host )
{
	const std::vector<Server> &servers = this->_socketFds[fd];

	for (size_t i = 0; i < servers.size(); i++)
	{
		if (servers[i].getServerName() == host)
			return (Server &)servers[i];
	}

	return (Server &)servers[0];
}

std::string myMismatch(std::vector<std::string> v, std::string s)
{
	std::map<int, std::string> m;

	for (size_t i = 0; i < v.size(); i++)
	{
		if (s == v[i])
			return v[i];
	}

	return s;
}

uint16_t checkUri(Request &req, Server &server)
{
	std::string uri = server.directives["root"][0];
	std::vector<std::string> v;

	for (size_t i = 0; i < server.context.size(); i++)
	{
		v.push_back(server.context[i].first);
	}
	std::string s = myMismatch(v, req.getUri());
	std::cerr << "--------------------------------------\n";
	std::cerr << (uri + s) << std::endl;
	std::cerr << "--------------------------------------\n";
	if (access((uri + s).c_str(), F_OK | R_OK) == -1)
	{
		perror("access");
		std::cerr << "Not found" << std::endl;
		return NOT_FOUND;
	}

	return OK; // TODO rediretion checking in location
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

bool Core::readRequest(int fd)
{
	char buffer[MAX_BUFFSIZE + 1];
	int readBytes;

	bzero(buffer, MAX_BUFFSIZE + 1);
	readBytes = recv(fd, buffer, MAX_BUFFSIZE, 0);
	if (readBytes == -1)
		return (this->requests[fd].status = INTERNAL_SERVER_ERROR, true);

	else if (readBytes == 0)
	{
		return false;
	}
	else
	{
		if (this->requests[fd].mainRequest(buffer, readBytes))
		{
			// Server &server = getServer(this->_FdsMap[fd], this->requests[fd].getUri());
			// this->requests[fd].status = checkUri(this->requests[fd], server);
			// this->requests[fd].setUri(server.directives["root"][0] + this->requests[fd].getUri());
			// this->responses[fd].setData(server, this->requests[fd]);
		}
	}
	return false;
}

std::string getDirUri(std::string &uri)
{
	std::string s = "<html><head><title>Index of " + uri + "</title></head><body><h1>Index of " + uri + "</h1><hr><pre>";
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(uri.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			s += "<a href=\"" + std::string(ent->d_name) + "\">" + ent->d_name + "</a><br>";
		}
		closedir(dir);
	}
	else
	{
		perror("");
		return "";
	}
	s += "</pre><hr></body></html>";
	return "Content-length: " + std::to_string(s.size()) + "\r\n" + s;
}

// std::string getTypeUri(std::string  uri)
// {
// 	struct stat sb;

// 	if (stat(uri.c_str(), &sb) == 0)
// 	{
// 		if (S_IFREG & sb.st_mode)
// 		{
// 			return ("Content-Type: " + getContentType(uri.substr(uri.find_last_of(".") + 1)) + "\r\nContent-length: " + std::to_string(test(uri).size()) + "\r\n\r\n" + test(uri));
// 		}
// 		else
// 		{
// 			return ("Content-Type: text/html\r\n" + getDirUri(uri));
// 		}
// 	}
// 	std::cout << uri << " error" << std::endl;
// 	return "";
// }

void Core::sendResponse(Request &req, int fd)
{
	// TODO: check Host, uri and method
	Server &server = getServer( fd, req.getHeaders()["host"] );

	if ( req.status == OK )
	{
		if (req.getMethod() == "GET")
		{
			req.status = checkUri(req, server);
			req.setUri(server.directives["root"][0] + req.getUri());
			this->responses[fd].setData(server, req);
			this->responses[fd].generateResponse();
			// std::string s = "HTTP/1.1 200 OK\r\n"
			// 				"Content-Type: text/plain\r\n"
			// 				"Content-Length: 12\r\n"
			// 				"\r\n"
			// 				"Hello world!";
			// send(fd, s.c_str(), s.length(), 0);
			// req.state = SENT;
			// std::cout << "response = " << response.getResponse() << std::endl;
			ssize_t result = send(fd, responses[fd].getResponse().c_str() + this->responses[fd].bytesSent, responses[fd].getResponse().length() - this->responses[fd].bytesSent, 0);
			if (result == -1)
			{
				std::cout << "send error" << std::endl;
				req.status = INTERNAL_SERVER_ERROR;
				return;
			}

			this->responses[fd].bytesSent += result;

			if ( this->responses[fd].bytesSent == this->responses[fd].getResponse().length() ) req.state = SENT;
		}
		else if (req.getMethod() == "POST")
		{
		}
		else if (req.getMethod() == "DELETE")
		{
		}
	}
	else
	{
		Error error(req.status);
		if ( req.status == BAD_REQUEST )
			send(fd, error.errorMessage("Bad Request").c_str(), error.errorMessage("Bad Request").length(), 0);
		else if ( req.status == NOT_FOUND )
			send(fd, error.errorMessage("Not Found").c_str(), error.errorMessage("Not Found").length(), 0);
		else if ( req.status == INTERNAL_SERVER_ERROR )
			send(fd, "HTTP/1.1 500 Internal Server Error\r\n", 36, 0);
		req.state = SENT;
	}

	return;
}

void Core::set_ports(int port)
{
	this->_ports.insert(port);
}
