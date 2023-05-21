/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Core.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 14:30:16 by moseddik          #+#    #+#             */
/*   Updated: 2023/05/21 16:47:24 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "WebServ.hpp"
#include "Request.hpp"


class Core
{
	private:
		std::map<int, std::vector<Server> > _core;
		std::map<int, std::vector<Server> > _socketFds;
		std::set<int> _ports;
		std::vector<struct pollfd> _pollfds;
		std::set<int> _pollFdsSet;

	public:
		Core( std::vector<Server> servers );
		~Core( void );

		void addServer( Server server );
		void addSocketFd( void );

		std::map<int, std::vector<Server> > const & get_core( void ) const;

		void set_ports( int port );
		bool find_port( int port );
		void set_pollfds( void );
		std::vector<struct pollfd> const & get_pollfds( void ) const;
		// int get_socketFd( void ) const;
		std::set<int> const & get_pollFdsSet( void ) const;
		std::map<int, Request> getRequests( void ) const;


		void	removeRequest(int fd, std::map<int, Request> &Requests);
		void	acceptConnection( std::map<int, Request> &Requests );
		bool	readRequest(int fd, std::map<int, Request> &req);
		void	sendResponse(Request &req, int fd);
};