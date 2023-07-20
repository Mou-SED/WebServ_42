/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/21 16:20:23 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/07/20 10:52:01 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "WebServ.hpp"
#include "Request.hpp"
#include "Core.hpp"
#include "Error.hpp"
#include "Utils.hpp"
#include "Cgi.hpp"

class Response
{
	private:
		Request 	_request;
		uint16_t 	_status;
		std::string _response;
		std::string _headers;
		std::string contentLength;
		off_t 		_bodySize;
		std::string _path;
		std::string _uri;

	public:
		int	cgi_passIndex;
		char *_buffer;
		std::string _Location;
		bool _isCGI;
		bool _isDir;
		std::vector<std::pair<std::set<int> , std::string> > _error_pages;
		std::ifstream ifs;
		size_t bytesSent;
		Response( void );
		Response( uint16_t status );
		~Response( void );

		std::string	getResponse( void ) const;
		void		toStringGet( void );
		void		toStringDelete( void );
		void		toStringPut( void );
		void		toString( std::string const &type);
		void		generateResponse( void );
		void		generateErrorResponse( void );
		bool		redirection(std::string &path,std::string const & uri, std::pair<std::string, Directives > * location);

		std::string	getDate( void ) const;
		std::string getHeaders( void ) const;
		std::string getPath( void ) const;
		off_t		getBodySize( void ) const;
		uint16_t	getStatus( void ) const;
		std::string getUri( void ) const;
		void 		clear( void );

		bool		checking_extension(std::string const &Path, std::vector<std::string> const & cgi);

		void		setErrorPages(std::vector<std::string> &error_pages);
		void 		setRequest( Request const & req );
		void 		setBodySize( std::string &path );
		void		setBodySize( off_t size );
		void 		setPath( std::string &path );
		void		setUri( std::string &uri );

		void GET( std::pair<std::string, Directives > * location );
		void DELETE( void );
		void PUT( void );
		void POST( std::pair<std::string, Directives > * location );
};