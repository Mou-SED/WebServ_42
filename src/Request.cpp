/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 17:07:46 by moseddik          #+#    #+#             */
/*   Updated: 2023/06/11 11:24:58 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(void)
{
	state = REQUEST_LINE;
	this->isChunked = false;
	this->contentLength = 0;
	this->status = OK;
	this->_server = nullptr;

	return;
}

std::string Request::getMethod(void) const
{
	return this->_method;
}

std::string Request::getUri(void) const
{
	return this->_uri;
}

std::map<std::string, std::string> Request::getHeaders(void) const
{
	return this->_headers;
}

UChar &Request::getBody(void) const
{
	return const_cast<UChar &>(this->_body);
}

void Request::clear(void)
{
	this->_request.clear();
	this->_method.clear();
	this->_uri.clear();
	this->_headers.clear();
	this->state = REQUEST_LINE;
	this->isChunked = false;
	this->status = 0;
}

uint16_t Request::getStatus(void) const
{
	return this->status;
}

Server * Request::getServer(void) const
{
	return this->_server;
}

std::string Request::getContentType()
{
	std::map<std::string, std::string> m;
	m["html"] = "text/html";
	m["htm"] = "text/html";
	m["jpg"] = "image/jpeg";
	m["jpeg"] = "image/jpeg";
	m["png"] = "image/png";
	m["gif"] = "image/gif";
	m["css"] = "text/css";
	m["js"] = "text/javascript";
	m["json"] = "application/json";
	m["xml"] = "application/xml";
	m["pdf"] = "application/pdf";
	m["zip"] = "application/zip";
	m["mp4"] = "video/mp4";
	m["webm"] = "video/webm";
	m["mp3"] = "audio/mp3";
	m["ogg"] = "audio/ogg";
	m["wav"] = "audio/wav";
	m["txt"] = "text/plain";
	m[""] = "application/octet-stream";
	if (m.find(this->_uri.substr(this->_uri.find_last_of(".") + 1)) != m.end())
		return m[this->_uri.substr(this->_uri.find_last_of(".") + 1)];
	else
		return m[""];
}

std::string Request::getContentLength(void)
{
	return std::to_string(this->_body.size());
}

bool findIf( std::map<std::string, std::string> &headers, std::vector<std::string> const v )
{
	for (size_t i = 0; i < v.size(); i++)
	{
		if (headers.find(v[i]) == headers.end())
			return true;
	}

	return false;
}

bool Request::parsingHeaders(std::vector<std::string> &tokens)
{
	size_t i = 0;

	for ( ; i < tokens.size() && tokens[i].find("\n") != std::string::npos; i++ )
	{
		std::pair<std::string, std::string> header;

		header = make_pair(
			tokens[i].substr(0, tokens[i].find(":")),
			tokens[i].substr(tokens[i].find(":") + 1)
		);
		std::transform(header.first.begin(), header.first.end(), header.first.begin(), ::tolower);

		if ( header.first.empty() or header.second.empty() or not findIf(this->_headers, {"content-length", "transfer-encoding"}) or not findIf(this->_headers, {header.first})
			or (not this->_headers["content-length"].empty() and not this->_headers["transfer-encoding"].empty()))
		{ // TODO : check headers can be duplicated in nginx.
			this->status = BAD_REQUEST;
			this->state = ERR;

			return false;
		}
		if ((tokens[i] == "\n" or tokens[i] == "\r\n") and (not this->_headers["content-length"].empty() or not this->_headers["transfer-encoding"].empty()) ) // TODO : check if the header is valid
		{
			i++;
			if ( this->_headers["transfer-encoding"] == "chunked" ) this->isChunked = true;
			if (not this->_headers["content-length"].empty()) this->contentLength = std::stoi(this->_headers["content-length"]);

			this->state = BODY;

			break;
		}
		else if ( tokens[i] == "\n" or tokens[i] == "\r\n" )
		{
			this->state = DONE;
			break;
		}
		this->_headers[header.first] = trim(header.second, " \r\n");
	}

	this->_request.clear();
	for (size_t j = i; j < tokens.size(); j++)
		this->_request.append(tokens[j]);

	return true;
}

void remove_end(std::string &str, std::string to_remove)
{
	size_t pos = str.find_first_of(to_remove);
	if (pos != std::string::npos)
		str = str.substr(0, pos);
}

bool checkMethod(std::string &line)
{
	std::vector<std::string> list = {"GET", "POST", "DELETE"};

	for (size_t i = 0; i < list.size(); i++)
	{
		if (line == list[i])
			return true;
	}

	return false;
}

bool Request::parsingStartLine(std::vector<std::string> &tokens)
{
	remove_end(tokens[0], "\r\n");
	std::vector<std::string> requestLine = split(tokens[0], ' ', false);

	if (requestLine.size() != 3 or not checkMethod(requestLine[0])) // TODO : check method should return another error (METHOD_NOT_IMPLEMENTED)
	{
		this->status = BAD_REQUEST;
		this->state = ERR;

		return false;
	}
	if (requestLine[2] != "HTTP/1.1")
	{
		this->status = VERSION_NOT_SUPPORTED;
		this->state = ERR;

		return false;
	}

	this->_method = requestLine[0];
	this->_uri = requestLine[1];
	this->state = HEADERS;
	tokens.erase(tokens.begin());

	return true;
}

bool Request::parsing(std::vector<std::string> &tokens)
{
	if ( state == REQUEST_LINE )
	{
		if ( not parsingStartLine(tokens) ) return false;
	}
	if ( state == HEADERS )
	{
		if ( not parsingHeaders(tokens) ) return false;
	}
	if ( state == BODY )
		return mainParsingRequest(const_cast<char *>(this->_request.toString().c_str()), this->_request.size());

	return true;
}

bool Request::readLine(std::string &line)
{
	if ( not this->_request.empty() )
	{
		this->_request += line;
		line = this->_request.toString();
	}

	std::vector<std::string> tokens = split(line, '\n', true);

	return parsing(tokens);
}

bool Request::parsingBody(char *buffer)
{
	if ( this->isChunked )
	{
		if ( not parsingChunked(buffer) ) return false;
	}
	else
	{
		_body += buffer;
		if (_body.size() == this->contentLength) this->state = DONE;
		else if (_body.size() > this->contentLength)
		{
			this->status = BAD_REQUEST;
			this->state = ERR;
			return false;
		}
	}
	return true;
}

void Request::setUri(std::string uri)
{
	this->_uri = uri;
}

void Request::setServer(Server *server)
{
	this->_server = server;
}

bool Request::mainParsingRequest(char *buffer, int size)
{
	std::string line(buffer, size);

	if ( this->state == BODY )
	{
		return parsingBody(buffer);
	}
	else if ( line.find("\n") == std::string::npos )
	{
		this->_request += line;
		return false;
	}
	else
		return ( readLine(line) );
}

bool Request::parsingChunked(char *buffer)
{
	if (not _chunkedTurn)
	{
		try
		{
			if ( std::string(buffer).find("\n") == std::string::npos ) return false;

			this->contentLength = std::stoi(std::string(buffer), 0, 16);
			if (this->contentLength == 0)
			{
				this->state = DONE;
				return true;
			}
			_chunkedTurn = true;
		}
		catch (const std::exception &e)
		{
			std::cerr << e.what() << '\n';
			this->status = BAD_REQUEST;
			this->state = ERR;
			return false;
		}
	}
	else
	{
		_chunkedBody += buffer;
		if (_chunkedBody.size() == this->contentLength)
		{
			_chunkedBody.clear();
			_chunkedTurn = false;
		}
		if (_chunkedBody.size() > this->contentLength)
		{
			this->status = BAD_REQUEST;
			this->state = ERR;
			return false;
		}
		this->_body += _chunkedBody;
	}

	return true;
}