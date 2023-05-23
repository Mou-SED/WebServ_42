/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 17:04:27 by moseddik          #+#    #+#             */
/*   Updated: 2023/05/23 12:47:10 by aaggoujj         ###   ########.fr       */
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
	DONE,
	ERR
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

public:
	state state;
	bool bodyCompleted;
	bool isChunked;
	uint16_t contentLength;
	uint16_t status;
	uint16_t isDone;
	Request(void);
	Request(uint16_t status);
	bool mainRequest(char *buffer, int size);
	bool ReadLine(std::string &line);
	bool parsingState(std::vector<std::string> &tokens);

	bool parsingChunked(char *buffer);
	bool parsingRequest(std::vector<std::string> &tokens);
	bool parsingHeaders(std::vector<std::string> &tokens);
	bool parsingBody(char *buffer);


	std::map<std::string, std::string> getHeaders(void) const;
	std::string getMethod(void) const;
	std::string getUri(void) const;
	UChar & getBody(void) const;

	void clear(void);
};
