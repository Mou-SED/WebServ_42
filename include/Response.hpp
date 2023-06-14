/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/21 16:20:23 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/06/14 18:25:30 by aaggoujj         ###   ########.fr       */
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
		UChar _body;
		Request _request;
		uint16_t _status;
		std::string _response;
		std::string _headers;
		std::string contentLength;
		off_t _bodySize;
		std::string _path;

	public:
		char *_buffer;
		std::ifstream ifs;
		size_t bytesSent;
		Response( void );
		Response( uint16_t status );
		~Response( void );

		std::string	getResponse( void ) const;
		void		toString( void );
		void		bodyToString( void );
		void		generateResponse( void );
		UChar		openUri( std::string path );

		std::string	getDate( void ) const;
		std::string getHeaders( void ) const;
		std::string getPath( void ) const;
		off_t		getBodySize( void ) const;
		static		std::string getStatusMessage( uint16_t status );
		void 		clear( void );

		void 		setRequest( Request const & req );
		void 		setBodySize( std::string &path );
		void 		setPath( std::string &path );

		void GET( void );
};