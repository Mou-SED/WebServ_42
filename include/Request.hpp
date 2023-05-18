/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 17:04:27 by moseddik          #+#    #+#             */
/*   Updated: 2023/05/18 15:46:40 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "WebServ.hpp"
#include "UChar.hpp"

enum state
{
	REQUEST_LINE,
	HEADERS,
	BODY,
	DONE
};

class Request
{
	private:
		UChar _request;
		std::string _method;
		std::string _uri;
		std::map<std::string, std::string> _headers; //TODO : only one header with the same name
		UChar	_body;
		UChar	_chunkedBody;

	public:
		state state;
		bool bodyCompleted;
		bool isChunked;
		uint16_t contentLength;
		uint16_t status;
		uint16_t isDone;
		Request( void );
		Request( uint16_t status );
		bool 	parseRequest(char * buffer, int size);
		bool	requestLine(std::string & line);
		bool	requestHeaders(std::vector<std::string> & lines);
		bool	requestBody(std::vector<std::string> & lines);
		bool	parsInState(std::vector<std::string> & tokens);

		bool	parsChunkedBody( void );

		bool isValid( void ) const;

		std::string getMethod(void) const;
		std::string getUri(void) const;
		std::map<std::string, std::string> getHeaders(void) const;
		std::string getBody(void) const;
		void	setState(std::string & line);
		void clear( void );
};

std::vector<std::string> split(std::string & str, char delim, bool keepIt);