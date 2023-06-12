/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 17:04:27 by moseddik          #+#    #+#             */
/*   Updated: 2023/06/11 11:24:35 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "WebServ.hpp"
#include "UChar.hpp"

enum State
{
	REQUEST_LINE,
	HEADERS,
	BODY,
	DONE,
	ERR,
	SENT
};

class Request
{
	private:
		UChar _request;
		std::string _method;
		std::string _uri;
		std::map<std::string, std::string> _headers; // TODO : only one header with the same name
		UChar _body;
		UChar _chunkedBody;
		bool _chunkedTurn = false;
		Server * _server;

	public:
		State state;
		bool bodyCompleted;
		bool isChunked;
		uint16_t contentLength;
		uint16_t status;
		Request(void);
		bool mainParsingRequest(char *buffer, int size);
		bool readLine(std::string &line);
		bool parsing(std::vector<std::string> &tokens);

		bool parsingChunked(char *buffer);
		bool parsingStartLine(std::vector<std::string> &tokens);
		bool parsingHeaders(std::vector<std::string> &tokens);
		bool parsingBody(char *buffer);

		void	setUri(std::string uri);
		void	setServer(Server * server);


		std::map<std::string, std::string> getHeaders(void) const;
		std::string getMethod(void) const;
		std::string getUri(void) const;
		UChar & getBody(void) const;
		std::string getContentType(void);
		std::string getContentLength(void);
		void clear(void);
		uint16_t getStatus( void ) const;
		Server * getServer( void ) const;
};
