/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 17:04:27 by moseddik          #+#    #+#             */
/*   Updated: 2023/06/24 00:09:51 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "WebServ.hpp"

enum State
{
	REQUEST_LINE,
	HEADERS,
	BODY,
	DONE,
	ERR,
	SENT
};

struct File
{
	std::string name;
	std::string path;
	std::fstream file;
	int WR_fd;

	File()
	{
		createName();
	}
	File & operator=(File const & rhs)
	{
		if (this != &rhs)
		{
			this->name = rhs.name;
			this->path = rhs.path;
			this->file.open(rhs.path.c_str(), std::fstream::in | std::fstream::out | std::fstream::trunc);
			this->WR_fd = rhs.WR_fd;
		}
		return *this;
	}
	void createName( void );
};


class Request
{
	private:
		std::stringstream _request;
		std::string _method;
		std::string _uri;
		std::map<std::string, std::string> _headers; // TODO : only one header with the same name
		std::stringstream _body;
		std::stringstream _chunkedBody;
		bool _chunkedTurn = false;
		Server * _server;

	public:
		File file;
		State state;
		bool bodyCompleted;
		bool isChunked;
		off_t _bodySize;
		unsigned long long  contentLength;
		uint16_t status;
		int chunkSize;
		Request(void);
		~Request(void);
		Request & operator=(const Request & src);
		bool mainParsingRequest(std::stringstream &ss);
		bool readLine(std::stringstream & line);
		bool parsing(std::vector<std::string> &tokens);

		bool parsingChunked(std::stringstream & buffer);
		bool parsingStartLine(std::vector<std::string> &tokens);
		bool parsingHeaders(std::vector<std::string> &tokens);
		bool parsingBody(std::stringstream &buffer);
		void	setRootDirectory(std::string root);
		void	setUri(std::string uri);
		void	setServer(Server * server);


		std::map<std::string, std::string> getHeaders(void) const;
		std::string getMethod(void) const;
		std::string getUri(void) const;
		std::string  getBody(void) const;
		std::string getContentType(void);
		std::string getContentLength(void);
		size_t getBodySize(void);
		void clear(void);
		uint16_t getStatus( void ) const;
		Server * getServer( void ) const;
};
