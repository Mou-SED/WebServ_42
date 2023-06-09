/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/21 16:20:23 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/06/09 11:59:27 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "WebServ.hpp"
#include "UChar.hpp"
#include "Request.hpp"
#include "Core.hpp"

class Response
{
	private:
		Server _server;
		Request _req;
		std::string _status;
		std::string _response;
		std::string contentLength;

	public:
		size_t bytesSent;
		Response( void );
		Response( std::string status );
		Response( Response const & src );
		Response( Server server, Request req );
		~Response( void );

		std::string	getResponse( void ) const;
		void		generateResponse( void );
		void		generateStatusLine( void );
		void		generateHeaders( void );
		void		generateBody( void );
		void		generateChunkedBody( void );
		std::string	getDate( void );
		std::string	getContentType(std::string ext);
		void 		clear( void );

		void		setData(Server server, Request req );
};