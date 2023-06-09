/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Core.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 15:20:59 by moseddik          #+#    #+#             */
/*   Updated: 2023/06/09 17:41:03 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Core.hpp"

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
					std::cerr << "Accept new connection" << std::endl;
				}
				else
				{
					// Read request
					std::cerr << "Read request" << std::endl;
				}
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

	fcntl( serverFd, F_SETFL, O_NONBLOCK );
	setsockopt( serverFd, SOL_SOCKET, SO_REUSEADDR, NULL, 0 );

	this->addPollFd( newFd );

	this->_serversByFd[newFd] = this->_serversByFd[serverFd];

	// TODO: Create request object

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
		setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, NULL, 0 );

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
