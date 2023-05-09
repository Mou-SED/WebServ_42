/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Core.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/09 14:30:16 by moseddik          #+#    #+#             */
/*   Updated: 2023/05/09 15:26:46 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "WebServ.hpp"

class Core
{
	private:
		std::map<int, std::vector<Server> > _core;
		std::map<int, std::vector<Server> > _socketFds;
		std::set<int> _ports;

	public:
		Core( std::vector<Server> servers );
		~Core( void );

		void addServer( Server server );
		void addSocketFd( void );

		std::map<int, std::vector<Server> > get_core( void ) const;

		void set_ports( int port );
		bool find_port( int port );
};