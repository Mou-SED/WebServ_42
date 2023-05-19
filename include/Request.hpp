/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 17:04:27 by moseddik          #+#    #+#             */
/*   Updated: 2023/05/19 19:53:05 by aaggoujj         ###   ########.fr       */
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
		bool	parsInState(std::vector<std::string> & tokens);

		bool	parsChunkedBody( void );
		bool	parsingRequest(std::vector<std::string> & tokens);
		bool	parsingHeaders(std::vector<std::string> & tokens);
		bool	parsingBody(std::vector<std::string> & tokens);

		bool isValid( void ) const;

		std::string getMethod(void) const;
		std::string getUri(void) const;
		std::map<std::string, std::string> getHeaders(void) const;
		std::string getBody(void) const;
		void	setState(std::string & line);
		void clear( void );
};

std::vector<std::string> split(std::string & str, char delim, bool keepIt);