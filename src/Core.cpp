/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Core.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: junik <abderrachidyassir@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 15:20:59 by moseddik          #+#    #+#             */
/*   Updated: 2023/07/17 17:59:01 by junik            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Core.hpp"
#include <cstring>

std::pair<uint32_t, uint16_t> Core::getHostAndPort( std::string const & host, std::string const & port )
{
	const struct addrinfo hint = (struct addrinfo){
		.ai_flags = 0,
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM,
		.ai_protocol = 0,
		.ai_addrlen = 0,
		.ai_addr = NULL,
		.ai_canonname = NULL,
		.ai_next = NULL,
	};

	struct addrinfo *res;
	int ret = getaddrinfo( host.c_str(), port.c_str(), &hint, &res );
	if ( ret != 0 )
		throw std::runtime_error( gai_strerror( ret ) );

	const struct sockaddr_in *addr = reinterpret_cast<struct sockaddr_in *>(res->ai_addr);

	return std::make_pair( addr->sin_addr.s_addr, addr->sin_port );
}

Core::Core(std::vector<Server> const &servers)
{
	std::map< std::pair< uint32_t, uint16_t >, int > serversByHost;

	for ( size_t i = 0; i < servers.size(); i++ )
	{
		std::pair < uint32_t, uint16_t > hostAndPort
			= getHostAndPort( servers[i].getHost(), std::to_string( servers[i].getPort() ) );

		if ( serversByHost.count( hostAndPort ) == 0 )
		{
			int fd = socket( AF_INET, SOCK_STREAM, 0 );
			if ( fd == -1 )
				throw std::runtime_error( strerror( errno ) );

			this->_serversByFd[fd].push_back( const_cast<Server *>(&servers[i]) );
			serversByHost[hostAndPort] = fd;
		}
		else // serversByHost.count( hostAndPort ) == 1
		{
			int fd = serversByHost[hostAndPort];
			this->_serversByFd[fd].push_back( const_cast<Server *>(&servers[i]) );
		}
	}

	typedef std::map< int, std::vector< Server *> >::iterator serversByFdIterator;
	for ( serversByFdIterator it = this->_serversByFd.begin(); it != this->_serversByFd.end(); it++ )
	{
		this->_listenFds.insert( it->first );
	}

	typedef std::set< int >::iterator listenFdsIterator;
	for ( listenFdsIterator it = this->_listenFds.begin(); it != this->_listenFds.end(); it++ )
	{
		this->addPollFd( *it );
	}

	this->bindServerSockets();

	return ;
}

void Core::addPollFd( int fd )
{
	struct pollfd pollFd = (struct pollfd){
		.fd = fd,
		.events = POLLIN | POLLOUT,
		.revents = 0
	};

	this->_pollFds.push_back( pollFd );
}

void Core::removePollFd( int fd )
{
	typedef std::vector< struct pollfd >::iterator pollFdsIterator;
	for ( pollFdsIterator it = this->_pollFds.begin(); it != this->_pollFds.end(); it++ )
	{
		if ( it->fd == fd )
		{
			this->_pollFds.erase( it );
			return ;
		}
	}
}

void Core::start( void )
{
	while ( 1337 )
	{
		int ret = poll( this->_pollFds.data(), this->_pollFds.size(), -1 );
		if ( ret == -1 )
			throw std::runtime_error( strerror( errno ) );

		int j = 0;
		for ( size_t i = 0; i < this->_pollFds.size() and j < ret; i++ )
		{
			const int currentFd = this->_pollFds[i].fd;

			if ( this->_pollFds[i].revents == 0 )
				continue ;
			j++;
			if ( this->_pollFds[i].revents & POLLIN )
			{
				if ( this->_listenFds.count( currentFd ) == 1 )
				{
					this->acceptNewConnection( currentFd );
				}
				else
				{
					this->readRequest( currentFd );
				}
			}
			else if ( this->_pollFds[i].revents & POLLOUT )
			{
				if ( this->_requests[currentFd].state == DONE or this->_requests[currentFd].state == ERR )
				{
					this->sentResponse( currentFd );
				}
			}
			if ( this->_requests[currentFd].state == SENT )
			{
				std::cerr << "Close connection on fd " << currentFd << std::endl;
				this->removePollFd( currentFd );
				this->_serversByFd.erase( currentFd );
				this->_requests[currentFd].clear();
				this->_requests.erase( currentFd );
				this->_responses[currentFd].clear();
				this->_responses.erase( currentFd );
				close( currentFd );
			}
		}
	}
	return ;
}

void Core::acceptNewConnection( int serverFd )
{
	//TODO: Save client info for later use (CGI)
	int newFd = accept( serverFd, NULL, NULL );
	if ( newFd == -1 )
		throw std::runtime_error( strerror( errno ) );

	std::cerr << "Accept new connection on fd " << newFd << std::endl;

	fcntl( serverFd, F_SETFL, O_NONBLOCK );
	int yes = 1;
	setsockopt( serverFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int) );

	this->addPollFd( newFd );

	this->_serversByFd[newFd] = this->_serversByFd[serverFd];

	this->_requests[newFd] = Request();

	return ;
}

void Core::bindServerSockets(void)
{
	typedef std::map< int, std::vector< Server *> >::iterator serversByFdIterator;
	for ( serversByFdIterator it = this->_serversByFd.begin(); it != this->_serversByFd.end(); it++ )
	{
		int const fd = it->first;
		Server * server = it->second.front();

		fcntl( fd, F_SETFL, O_NONBLOCK );

		const int yes = 1;
		setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int) );

		const struct addrinfo hint = (struct addrinfo){
			.ai_flags = 0,
			.ai_family = AF_INET,
			.ai_socktype = SOCK_STREAM,
			.ai_protocol = 0,
			.ai_addrlen = 0,
			.ai_addr = NULL,
			.ai_canonname = NULL,
			.ai_next = NULL,
		};

		struct addrinfo *res;
		int ret = getaddrinfo( server->getHost().c_str(), std::to_string( server->getPort() ).c_str(), &hint, &res );
		if ( ret != 0 )
			throw std::runtime_error( gai_strerror( ret ) );

		ret = bind( fd, res->ai_addr, res->ai_addrlen );
		if ( ret == -1 )
			throw std::runtime_error( strerror( errno ) );
		freeaddrinfo( res );

		ret = listen( fd, SOMAXCONN );
		if ( ret == -1 )
			throw std::runtime_error( strerror( errno ) );
	}
}

Server * Core::getServer( int fd, const std::string & host )
{
	const std::vector<Server *> & servers = this->_serversByFd[fd];

	for (size_t i = 0; i < servers.size(); i++)
	{
		if (servers[i]->getServerName() == host)
			return servers[i];
	}

	return servers[0];
}

void Core::readRequest( int clientFd )
{
	char buffer[MYBUFSIZ];

	ssize_t readBytes = recv( clientFd, buffer, MYBUFSIZ, 0 );
	if ( readBytes == -1  ) return ;

	Request &request = this->_requests[clientFd];
	if ( readBytes == 0 )
	{
		// INFO: The client has closed the connection
		request.state = SENT;
		return ;
	}
	std::stringstream ss;
	ss.write(buffer, readBytes);
	request.mainParsingRequest( ss );
	if ( request.state == DONE )
	{
		request.setServer( getServer( clientFd, request.getHeaders()["host"] ) );
	}

	return ;
}

void Core::sentGetResponse(int clientFd)
{
	if ( this->_responses.count(clientFd) == 0 )
	{
		Response & response = this->_responses[clientFd];
		Request & request = this->_requests[clientFd];
		response.setRequest( request );
		response.setErrorPages( request.getServer()->directives["error_page"] );
		response.generateResponse();
		std::cerr << response.getHeaders() << std::endl;
		send( clientFd, response.getHeaders().c_str(), response.getHeaders().length(), 0);
		if (response._Location != "")
		{
			this->_requests[clientFd].state = SENT;
			return ;
		}
		if ( response.getStatus() < BAD_REQUEST and response._isDir == false and response._isCGI == false)
		{
			std::cerr << "Errrrrrrrrrrooorr\n";
			response.ifs.open( response.getPath().c_str() ); // TODO: Close file after
			this->_responses[clientFd]._buffer = new char [response.getBodySize() + 1];
			memset(this->_responses[clientFd]._buffer, 0, response.getBodySize() + 1);
			this->_responses[clientFd].ifs.read (this->_responses[clientFd]._buffer,response.getBodySize());
		}
	}
	ssize_t sentBytes = send(
		clientFd,
		this->_responses[clientFd]._buffer + this->_responses[clientFd].bytesSent,
		this->_responses[clientFd].getBodySize() - (off_t)this->_responses[clientFd].bytesSent,
		0
	);

	this->_responses[clientFd].bytesSent += sentBytes;
	if ( (off_t)this->_responses[clientFd].bytesSent == this->_responses[clientFd].getBodySize())
	{
		this->_requests[clientFd].state = SENT;
		std::cerr << "Sent" << std::endl;
	}

	return ;
}

void Core::sentHeadersResponse( int clientFd )
{
	Response & response = this->_responses[clientFd];
	Request & request = this->_requests[clientFd];
	response.setRequest( request );
	response.generateResponse();
	std::cerr << "headers sent = " << response.getHeaders() << std::endl;
	send( clientFd, response.getHeaders().c_str(), response.getHeaders().length(), 0);
	if ( response.getStatus() < BAD_REQUEST )
	{
		response.ifs.open( response.getPath().c_str() ); // TODO: Close file after
		this->_responses[clientFd]._buffer = new char [response.getBodySize() + 1];
		memset(this->_responses[clientFd]._buffer, 0, response.getBodySize() + 1);
		this->_responses[clientFd].ifs.read (this->_responses[clientFd]._buffer,response.getBodySize());
	}
	std::cerr << "buffer sent = " << this->_responses[clientFd]._buffer << std::endl;
	ssize_t sentBytes = send(
		clientFd,
		this->_responses[clientFd]._buffer + this->_responses[clientFd].bytesSent,
		this->_responses[clientFd].getBodySize() - (off_t)this->_responses[clientFd].bytesSent,
		0
	);

	this->_responses[clientFd].bytesSent += sentBytes;
	if ( (off_t)this->_responses[clientFd].bytesSent == this->_responses[clientFd].getBodySize())
		this->_requests[clientFd].state = SENT;
}

void Core::sentPostResponse( int clientFd )
{
	Response & response = this->_responses[clientFd];
	Request & request = this->_requests[clientFd];
	response.setRequest( request );
	response.generateResponse();
	send( clientFd, response.getHeaders().c_str(), response.getHeaders().length(), 0);
	ssize_t sentBytes = send(
		clientFd,
		this->_responses[clientFd]._buffer + this->_responses[clientFd].bytesSent,
		this->_responses[clientFd].getBodySize() - (off_t)this->_responses[clientFd].bytesSent,
		0
	);

	this->_responses[clientFd].bytesSent += sentBytes;
	if ( (off_t)this->_responses[clientFd].bytesSent == this->_responses[clientFd].getBodySize())
	{
		std::cerr << "Sent!!!!" << std::endl;
		this->_requests[clientFd].state = SENT;
	}
}

void Core::sentResponse( int clientFd )
{
	if ( this->_requests[clientFd].state == ERR )
	{
		Error error( this->_requests[clientFd].status );
		send( clientFd, error.getErrorMessage().c_str(), error.getErrorMessage().length(), 0 );
		this->_requests[clientFd].state = SENT;
		return ;
	}
	if ( this->_requests[clientFd].getMethod() == "GET" )
		this->sentGetResponse( clientFd );
	else if ( this->_requests[clientFd].getMethod() == "DELETE" )
		this->sentHeadersResponse( clientFd );
	else if ( this->_requests[clientFd].getMethod() == "PUT" )
		this->sentHeadersResponse( clientFd );
	else if ( this->_requests[clientFd].getMethod() == "POST" )
		this->sentPostResponse( clientFd );
}

