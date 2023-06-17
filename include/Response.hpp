/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/21 16:20:23 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/06/17 13:59:09 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "WebServ.hpp"
#include "UChar.hpp"
#include "Request.hpp"
#include "Core.hpp"
#include "Error.hpp"

class Response
{
	private:
		UChar 		_body;
		Request 	_request;
		uint16_t 	_status;
		std::string _response;
		std::string _headers;
		std::string contentLength;
		off_t 		_bodySize;
		std::string _path;
		// Error 		_error;

	public:
		char *_buffer;
		std::string _Location = "";
		bool _isCGI = false;
		bool _isDir = false;
		std::vector<std::pair<std::set<int> , std::string> > _error_pages;
		std::ifstream ifs;
		size_t bytesSent;
		Response( void );
		Response( uint16_t status );
		~Response( void );

		std::string	getResponse( void ) const;
		void		toStringGet( void );
		void		toStringDelete( void );
		void		toString( std::string const &type);
		void		bodyToString( void );
		void		generateResponse( void );
		bool		redirection(std::string &path,std::string &uri, std::pair<std::string, Directives > * location);

		std::string	getDate( void ) const;
		std::string getHeaders( void ) const;
		std::string getPath( void ) const;
		off_t		getBodySize( void ) const;
		static		std::string getStatusMessage( uint16_t status );
		uint16_t	getStatus( void ) const;
		void 		clear( void );

		void		setErrorPages(std::vector<std::string> &error_pages);
		void 		setRequest( Request const & req );
		void 		setBodySize( std::string &path );
		void		setBodySize( off_t size );
		void 		setPath( std::string &path );

		void GET( void );
		void DELETE( void );
};