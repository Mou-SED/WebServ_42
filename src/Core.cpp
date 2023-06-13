/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Core.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 15:20:59 by moseddik          #+#    #+#             */
/*   Updated: 2023/06/13 16:43:04 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Core.hpp"

unsigned int BUFSIZE = 4294967295;

std::pair<uint32_t, uint16_t> Core::getHostAndPort( std::string const & host, std::string const & port )
{
	const struct addrinfo hint = (struct addrinfo){
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM,
		.ai_protocol = 0
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
					try
					{
						this->readRequest( currentFd );
					}
					catch(const std::exception& e)
					{
						this->removePollFd( currentFd );
						this->_serversByFd.erase( currentFd );
						this->_requests.erase( currentFd );
						this->_responses.erase( currentFd );
						close( currentFd );
					}
				}
			}
			else if ( this->_pollFds[i].revents & POLLOUT )
			{
				if ( this->_requests[currentFd].state == DONE or this->_requests[currentFd].state == ERR )
				{
					// std::cerr << "Send response" << std::endl;
					this->sentResponse( currentFd );
				}
			}
			if ( this->_requests[currentFd].state == SENT )
			{
				this->removePollFd( currentFd );
				this->_serversByFd.erase( currentFd );
				this->_requests.erase( currentFd );
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
			.ai_family = AF_INET,
			.ai_socktype = SOCK_STREAM,
			.ai_protocol = 0
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
	char buffer[BUFSIZ + 1];

	ssize_t readBytes = recv( clientFd, buffer, BUFSIZ, 0 );
	if ( readBytes == -1  )
	{
		std::cerr << "Ana sbab fhadchi" << std::endl;
		throw std::runtime_error( strerror( errno ) );
	}

	buffer[readBytes] = '\0';

	Request &request = this->_requests[clientFd];
	if ( readBytes == 0 )
	{
		std::cerr << "I'm Here" << std::endl;
		// INFO: The client has closed the connection
		request.state = SENT;
	}
	request.mainParsingRequest( buffer, readBytes );
	if ( request.state == DONE )
	{
		std::cerr<< "Hello Server" << std::endl;
		request.setServer( getServer( clientFd, request.getHeaders()["host"] ) );
	}

	return ;
}

void Core::sentResponse( int clientFd )
{
	if ( this->_responses.count(clientFd) == 0 )
	{
		Response & response = this->_responses[clientFd];
		Request & request = this->_requests[clientFd];
		response.setRequest( request );
		response.generateResponse();
		response.toString();
	}

	std::string responseStr = this->_responses[clientFd].getResponse();


	ssize_t sentBytes = send(
		clientFd,
		responseStr.c_str() + this->_responses[clientFd].bytesSent,
		std::min((size_t)BUFSIZE, responseStr.length() - this->_responses[clientFd].bytesSent),
		0
	);
	if ( sentBytes == -1 )
		throw std::runtime_error( strerror( errno ) );
	//TODO : Remove this
	// std::cerr << "bytesSent = " << this->_responses[clientFd].bytesSent << std::endl;
	// std::cerr << "responseStr.length() = " << responseStr.length() << std::endl;
	// std::cerr << "------------------------------------------" << std::endl;

	this->_responses[clientFd].bytesSent += sentBytes;
	if ( this->_responses[clientFd].bytesSent == responseStr.length() )
		this->_requests[clientFd].state = SENT;
	return ;
}
