/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/21 16:20:23 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/06/10 19:44:52 by moseddik         ###   ########.fr       */
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
		Request _request;
		uint16_t _status;
		std::string _response;
		std::string contentLength;

	public:
		size_t bytesSent;
		Response( void );
		Response( uint16_t status );
		Response( Request const & req );
		~Response( void );

		std::string	getResponse( void ) const;
		void		toString( void );
		void		statusLineToStirng( void );
		void		headersToString( void );
		void		bodyToString( void );

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