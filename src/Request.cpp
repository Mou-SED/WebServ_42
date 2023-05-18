/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 17:07:46 by moseddik          #+#    #+#             */
/*   Updated: 2023/05/18 17:54:40 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request( void )
{
	state = REQUEST_LINE;
	this->isChunked = false;
	this->contentLength = 0;

	return ;
}

Request::Request( uint16_t status )
{
	this->status = status;

	return ;
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

void Request::setState(std::string & line)
{
	(void)line;
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

bool Request::parsInState(std::vector<std::string> & tokens)
{
	std::string s[4] = {"REQUEST_LINE", "HEADERS", "BODY", "DONE"};
	std::cout << "parsInState" << std::endl;
	std::cout << "state : " << s[state] << std::endl;
	if ( state == REQUEST_LINE )
	{
		std::cout << "REQUEST_LINE" << std::endl;
		tokens[0] = tokens[0].substr(0,tokens[0].find_first_of("\r"));
		tokens[0] = tokens[0].substr(0,tokens[0].find_first_of("\n"));
		std::vector<std::string> requestLine = split(tokens[0], ' ', false);
		if (requestLine.size() != 3)
		{
			std::cout << "requestLine.size() != 3" << std::endl;
			this->status = 400;
			return false;
		}
		if (requestLine[2] != "HTTP/1.1")
		{
			std::cout << "requestLine[2] != \"HTTP/1.1\"" << std::endl;
			this->status = 505;
			return false;
		}
		this->_method = requestLine[0];
		this->_uri = requestLine[1];
		this->state = HEADERS;
		tokens.erase(tokens.begin());
	}
	if ( state == HEADERS )
	{
		size_t i = 0;
		std::cout << "HEADERS" << std::endl;
		for (i = 0; i < tokens.size() && tokens[i].find("\n") != std::string::npos; i++)
		{
			std::string key, value;
			key = tokens[i].substr(0, tokens[i].find(":"));
			value = tokens[i].substr(tokens[i].find(":") + 1);
			std::transform(key.begin(), key.end(), key.begin(), ::tolower);
			if ( (tokens[i] == "\n" or tokens[i] == "\r\n") and ( not this->_headers["content-length"].empty() or this->_headers["transfer-encoding"] == "chunked" ) ) //TODO : check if the header is valid
			{
				i++;
				std::cout << "-------------------------------" << std::endl;
				if (this->_headers["transfer-encoding"] == "chunked")
				{
					std::cout << "is chunked" << std::endl;
					this->isChunked = true;
				}
				else if (not this->_headers["content-length"].empty())
					this->contentLength = std::stoi(this->_headers["content-length"]);
				this->state = BODY;
				break;
			}
			else if (tokens[i] == "\n" or tokens[i] == "\r\n")
			{
				std::cout << "iss doone\n";
				this->state = DONE;
				break;
			}
			if (key.empty() or value.empty())
			{
				std::cout << "key.empty() or value.empty()" << std::endl;
				this->status = 400;
				return false;
			}
			this->_headers[key] = trim(value, " \r\n");
		}
		this->_request.clear();
		for (size_t j = i; j < tokens.size(); j++)
			this->_request.append(tokens[j]);
		for (auto it : this->_headers)
			std::cout << it.first << " : " << it.second << std::endl;
	}
	if (state == BODY)
		return parseRequest(const_cast<char *>(this->_request.toString().c_str()), this->_request.size());
	return true;
}

bool	Request::requestLine(std::string & line)
{
	if (not this->_request.empty())
	{
		this->_request += line;
		line = this->_request.toString();
	}
	std::vector<std::string> tokens = split(line, '\n', true);
	return parsInState(tokens);
}

bool	Request::parseRequest(char * buffer, int size)
{
	std::string line(buffer, size);

	if (this->state == BODY)
	{
		_body += buffer;
		if (this->isChunked)
		{
			if (not parsChunkedBody())
				return false;
		}
		else
		{
			if (_body.size() >= this->contentLength)
			{
				std::cout << "####################################body########################\n " << _body.toString() << std::endl;
				this->state = DONE;
				this->isDone = true;
			}
		}
	}
	else if ( line.find("\n") == std::string::npos )
	{
		this->_request += line;
		return false;
	}
	else
		return ( requestLine(line) );
	return true;
}



bool	Request::parsChunkedBody( void )
{
	size_t i;
	std::cout << "parsChunkedBody" << std::endl;
	if (_body.empty())
	{
		std::cout << "body is empty" << std::endl;
		return false;
	}
	std::cout << "body : " << _body.toString() << std::endl;
	int chunkSize = std::stoi(_body.toString(), &i, 16);
	std::cout << "chunkSize : " << chunkSize << std::endl;
	if (chunkSize == 0)
	{
		this->state = DONE;
		return true;
	}
	else
	{
		if (_body[i] == '\r')
			this->_chunkedBody += _body.toString().substr(i + 2, chunkSize);
		else
			this->_chunkedBody += _body.toString().substr(i + 1, chunkSize);
		if (_body[i + chunkSize + 1] == '\r')
			_body = _body.toString().substr(i + chunkSize + 3);
		else
			_body = _body.toString().substr(i + chunkSize + 2);
		return parsChunkedBody();
	}
	return true;
}

