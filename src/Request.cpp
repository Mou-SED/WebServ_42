/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 17:07:46 by moseddik          #+#    #+#             */
/*   Updated: 2023/06/17 21:20:40 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

void	checkHeaders(std::map<std::string, std::string> &headers)
{
	if (headers["host"].empty())
		throw std::runtime_error("Host header is missing");
}


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

std::string Request::getBody(void) const
{
	return (this->_body.str());
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
	return std::to_string(this->_body.str().size());
}

size_t Request::getBodySize(void)
{
	return this->_body.str().size();
}

Request & Request::operator=(Request const & rhs)
{
	this->_request << rhs._request.str();
	this->_method = rhs._method;
	this->_uri = rhs._uri;
	this->_headers = rhs._headers;
	this->_body << rhs._body.str();
	this->state = rhs.state;
	this->isChunked = rhs.isChunked;
	this->contentLength = rhs.contentLength;
	this->status = rhs.status;
	this->_server = rhs._server;

	return *this;
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

bool checkHeaders(std::map<std::string, std::string> headers, std::string key)
{
	if (headers.count(key) != 0 and headers[key].empty())
		return false;
	return true;
}

bool Request::parsingHeaders(std::vector<std::string> &tokens)
{
	size_t i = 0;

	for ( ; i < tokens.size() && tokens[i].find("\n") != std::string::npos; i++ )
	{
		std::pair<std::string, std::string> header;

		header = make_pair(
			tokens[i].substr(0, tokens[i].find(":")),
			tokens[i].substr(tokens[i].find(":") + 1, tokens[i].find("\n") - tokens[i].find(":") - 1)
		);
		std::transform(header.first.begin(), header.first.end(), header.first.begin(), ::tolower);

		if ( header.first.empty() or header.second.empty() or not findIf(this->_headers, {header.first}) or not checkHeaders(this->_headers, "content-length") or not checkHeaders(this->_headers, "transfer-encoding"))
		{ // TODO : check headers can be duplicated in nginx.

			std::cerr << header.first << " " << header.second << std::endl;
			this->status = BAD_REQUEST;
			this->state = ERR;
			return false;
		}
		if ((tokens[i] == "\n" or tokens[i] == "\r\n") and (not this->_headers["content-length"].empty() or not this->_headers["transfer-encoding"].empty()) ) // TODO : check if the header is valid
		{
			i++;
			if ( this->_headers["transfer-encoding"] == "chunked" ) this->isChunked = true;
			if (not this->_headers["content-length"].empty()) this->contentLength = std::stoll(this->_headers["content-length"]);
			this->state = BODY;
			break;
		}
		else if ( tokens[i] == "\n" or tokens[i] == "\r\n" )
		{
			this->state = DONE;
			checkHeaders(this->_headers);
			break;
		}
		std::cerr << header.first << " " << header.second << std::endl;
		this->_headers[header.first] = trim(header.second, " \r\n");
	}

	this->_request.clear();
	for (size_t j = i; j < tokens.size(); j++)
	{
		// if (i != 0 and tokens[i].find("Content") != std::string::npos)
			this->_request << tokens[j];
	}

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
	std::vector<std::string> list = {"GET", "POST", "DELETE", "PUT"};

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
		if ( requestLine[0] == "OPTIONS" or requestLine[0] == "HEAD" or requestLine[0] == "PATCH" )
			this->status = NOT_IMPLEMENTED;
		else
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
	std::cerr << "method : " << this->_method << std::endl;
	std::cerr << "uri : " << this->_uri << std::endl;
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
		return mainParsingRequest(this->_request);

	return true;
}

bool Request::readLine(std::stringstream &line)
{
	if ( not this->_request.str().empty() )
	{
		this->_request << line.str();
		line << this->_request.str();
	}

	std::vector<std::string> tokens = split((line.str()), '\n', true);

	return parsing(tokens);
}

bool Request::parsingBody(std::stringstream &buffer)
{
	if ( this->isChunked )
	{
		if ( not parsingChunked(buffer) ) return false;
	}
	else
	{
		_body << buffer.str();
		std::cerr << "before : " << _body.str().size() << std::endl;
		std::cerr << "contentLength : " << this->contentLength << std::endl;
		// std::cerr << "len = " << strlen(buffer) << std::endl;
		if (_body.str().size() == this->contentLength)
		{
			this->state = DONE;
			// std::cerr << "body : " << _body.str() << std::endl;
		}
		else if (_body.str().size() > this->contentLength)
		{
			std::cerr << "body : " << _body.str().size() << std::endl;
			std::cerr << "contentLength : " << this->contentLength << std::endl;
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

bool Request::mainParsingRequest(std::stringstream &ss)
{

	if ( this->state == BODY )
	{
		checkHeaders(this->_headers);
		return parsingBody(ss);
	}
	else if ( ss.str().find("\n") == std::string::npos )
	{
		this->_request << ss.str();
		return false;
	}
	else
		return ( readLine(ss) );
}

bool Request::parsingChunked(std::stringstream &buffer)
{
	if (not _chunkedTurn)
	{
		try
		{
			if (buffer.str().find("\n") == std::string::npos ) return false;

			this->contentLength = std::stoi(buffer.str(), 0, 16);
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
		_chunkedBody << buffer.str();
		if (_chunkedBody.str().size() == this->contentLength)
		{
			_chunkedBody.clear();
			_chunkedTurn = false;
		}
		if (_chunkedBody.str().size() > this->contentLength)
		{
			this->status = BAD_REQUEST;
			this->state = ERR;
			return false;
		}
		this->_body << _chunkedBody.str();
	}

	return true;
}