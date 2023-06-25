/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Core.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 14:30:16 by moseddik          #+#    #+#             */
/*   Updated: 2023/06/25 17:44:23 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "WebServ.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Response;

class Core
{
	private:
		std::map< int, std::vector<Server *> > _serversByFd;
		std::set< int > _listenFds;
		std::vector< struct pollfd > _pollFds;
		std::map< int, Request > _requests;
		std::map< int, Response > _responses;

	public:
		Core( std::vector< Server > const & servers );

		std::pair<uint32_t, uint16_t> getHostAndPort( std::string const & host, std::string const & port );
		void	addPollFd( int fd );
		void	removePollFd( int fd );
		void	start( void );
		void	acceptNewConnection( int serverFd );
		void	bindServerSockets( void );
		void	readRequest( int clientFd );

		void	sentResponse( int clientFd );
		void	sentGetResponse( int clientFd );
		void	sentHeadersResponse( int clientFd );
		void	sentPostResponse( int clientFd );
		Server * getServer( int fd, const std::string & host );
};