/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/21 16:20:23 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/06/12 16:14:03 by moseddik         ###   ########.fr       */
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
		std::string contentLength;

	public:
		size_t bytesSent;
		Response( void );
		Response( uint16_t status );
		~Response( void );

		std::string	getResponse( void ) const;
		void		toString( void );
		void		statusLineToStirng( void );
		void		headersToString( void );
		void		bodyToString( void );

		void		generateResponse( void );
		UChar		openUri( std::string path );

		void		generateChunkedBody( void );
		std::string	getDate( void );
		std::string	getContentType(std::string ext);
		void 		clear( void );

		void 		setRequest( Request const & req );

		static		std::string getStatusMessage( uint16_t status );

		void GET( void );
};