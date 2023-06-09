/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Core.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 14:30:16 by moseddik          #+#    #+#             */
/*   Updated: 2023/06/09 11:51:52 by aaggoujj         ###   ########.fr       */
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
		std::map<int, std::vector<Server> > _core;
		std::map<int, std::vector<Server> > _socketFds;
		std::set<int> _ports;
		std::vector<struct pollfd> _pollfds;

	public:
		std::map<int, int> _FdsMap;
		std::set<int> _pollFdsSet;
		std::map<int, Request> requests;
		std::map<int, Response> responses;
		Core( std::vector<Server> servers );
		~Core( void );

		void addServer( Server server );
		void addSocketFd( void );

		std::map<int, std::vector<Server> > const & get_core( void ) const;
		std::map<int, Response> const & get_responses( void ) const;
		void set_ports( int port );
		bool find_port( int port );
		void set_pollfds( void );
		std::vector<struct pollfd> const & get_pollfds( void ) const;
		// int get_socketFd( void ) const;
		std::set<int> const & get_pollFdsSet( void ) const;
		// std::map<int, Request> getRequests( void ) const;
		Server	& getServer(int fd, const std::string & host );
		void	closeConnection(int fd, Request &req);
		void	cleanResponse(int fd);
		void	removeRequest(int fd, std::map<int, Request> &Requests);
		void	acceptConnection( const struct pollfd &pfd, int serverFd );
		bool	readRequest(int fd);
		void	sendResponse(Request &req, int fd);
};