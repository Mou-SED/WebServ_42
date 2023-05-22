/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 17:07:46 by moseddik          #+#    #+#             */
/*   Updated: 2023/05/22 17:04:42 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(void)
{
	state = REQUEST_LINE;
	this->isChunked = false;
	this->contentLength = 0;
	this->status = 200;

	return;
}

Request::Request(uint16_t status)
{
	this->status = status;

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

bool findIf(std::map<std::string, std::string> &headers, std::vector<std::string> const v)
{
	for (size_t i = 0; i < v.size(); i++)
	{
		std::cout << "find ==== : "  << v[i] << std::endl;
		if (headers.find(v[i]) == headers.end())
			return true;
	}
	std::cout << "findIf : " << v.size() <<  " " << v[0] << std::endl;
	return false;
}

bool Request::parsingHeaders(std::vector<std::string> &tokens)
{
	size_t i = 0;
	for (i = 0; i < tokens.size() && tokens[i].find("\n") != std::string::npos; i++)
	{
		std::pair<std::string, std::string> header;
		header = make_pair(tokens[i].substr(0, tokens[i].find(":")), tokens[i].substr(tokens[i].find(":") + 1));
		std::transform(header.first.begin(), header.first.end(), header.first.begin(), ::tolower);
		if (header.first.empty() or header.second.empty() or not findIf(this->_headers, {"content-length", "transfer-encoding"}) or not findIf(this->_headers, {header.first}))
		{
			this->status = 400;
			this->state = ERR;
			return false;
		}
		if ((tokens[i] == "\n" or tokens[i] == "\r\n") and (not this->_headers["content-length"].empty() or this->_headers.find("transfer-encoding") != this->_headers.end())) // TODO : check if the header is valid
		{
			i++;
			if (this->_headers.find("transfer-encoding") != this->_headers.end() and this->_headers["transfer-encoding"] == "chunked")
			{
				std::cout << "is chunked" << std::endl;
				this->isChunked = true;
			}
			else if (not this->_headers["content-length"].empty())
			{
				std::cout << "is Body" << std::endl;
				this->contentLength = std::stoi(this->_headers["content-length"]);
			}
			this->state = BODY;
			break;
		}
		else if (tokens[i] == "\n" or tokens[i] == "\r\n")
		{
			std::cout << "is doone\n";
			this->state = DONE;
			break;
		}
		this->_headers[header.first] = trim(header.second, " \r\n");
	}
	this->_request.clear();
	for (size_t j = i; j < tokens.size(); j++)
		this->_request.append(tokens[j]);
	for (auto it : this->_headers)
		std::cout << it.first << " : " << it.second << std::endl;
	return true;
}

void remove_end(std::string &str, std::string to_remove)
{
	size_t pos = str.find_first_of(to_remove);
	if (pos != std::string::npos)
		str = str.substr(0, pos);
}

bool check_list(std::string &line, std::vector<std::string> list)
{
	for (size_t i = 0; i < list.size(); i++)
	{
		if (line == list[i])
			return true;
	}
	return false;
}

bool Request::parsingRequest(std::vector<std::string> &tokens)
{
	remove_end(tokens[0], "\r\n");
	std::cout << "tokens[0] == " << tokens[0] << std::endl;
	std::vector<std::string> requestLine = split(tokens[0], ' ', false);
	if (requestLine.size() != 3 or not check_list(requestLine[0], {"GET", "POST", "DELETE"}))
	{
		std::cout << "size == " << requestLine.size() << std::endl;
		std::cout << "tokens[0] == " << tokens[0] << std::endl;
		std::cout << "requestLine.size() != 3" << std::endl;
		this->status = 400;
		this->state = ERR;
		return false;
	}
	if (requestLine[2] != "HTTP/1.1")
	{
		std::cout << requestLine[2] << " != \"HTTP/1.1\"" << std::endl; //! ERROR
		this->status = 505;
		this->state = ERR;
		return false;
	}
	this->_method = requestLine[0];
	this->_uri = requestLine[1];
	this->state = HEADERS;
	tokens.erase(tokens.begin());
	return true;
}

bool Request::parsingState(std::vector<std::string> &tokens)
{
	std::string s[4] = {"REQUEST_LINE", "HEADERS", "BODY", "DONE"};
	std::cout << "parsingState" << std::endl;
	std::cout << "state : " << s[state] << std::endl;
	if (state == REQUEST_LINE)
	{
		if (not parsingRequest(tokens))
			return false;
	}
	if (state == HEADERS)
	{
		if (not parsingHeaders(tokens))
		{
			std::cout << "not parsingHeaders" << std::endl;
			return false;
		}
	}
	if (state == BODY)
		return mainRequest(const_cast<char *>(this->_request.toString().c_str()), this->_request.size());
	return true;
}

bool Request::ReadLine(std::string &line)
{
	if (not this->_request.empty())
	{
		this->_request += line;
		line = this->_request.toString();
	}
	std::vector<std::string> tokens = split(line, '\n', true);
	return parsingState(tokens);
}

bool Request::parsingBody(char *buffer)
{
	if (this->isChunked)
	{
		if (not parsingChunked(buffer))
			return false;
	}
	else
	{
		_body += buffer;
		if (_body.size() == this->contentLength)
		{
			this->state = DONE;
			this->isDone = true;
		}
		else if (_body.size() > this->contentLength)
		{
			std::cout << "+" << _body.toString() << "+" << std::endl;
			this->status = 400;
			this->state = ERR;
			return false;
		}
	}
	return true;
}

bool Request::mainRequest(char *buffer, int size)
{
	std::string line(buffer, size);

	if (this->state == BODY)
	{
		// std::cout << "body : " << _body.toString() << std::endl;
		return  parsingBody(buffer);
	}
	else if (line.find("\n") == std::string::npos)
	{
		this->_request += line;
		return false;
	}
	else
		return (ReadLine(line));
	return true;
}

bool Request::parsingChunked(char * buffer)
{
	if (not _chunkedTurn)
	{
		try
		{
			if (std::string(buffer).find("\n") == std::string::npos)
				return false;
			this->contentLength = std::stoi(std::string(buffer), 0, 16);
			if (this->contentLength == 0)
			{
				this->state = DONE;
				this->isDone = true;
				return true;
			}
			_chunkedTurn = true;
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			this->status = 400;
			this->state = ERR;
			return false;
		}
	}
	else
	{
		_chunkedBody += buffer;
		std::cout << "chunkedBody : " << _chunkedBody.toString() << std::endl;
		if (_chunkedBody.size() == this->contentLength)
		{
			_chunkedBody.clear();
			_chunkedTurn = false;
		}
		if (_chunkedBody.size() > this->contentLength)
		{
			std::cout << "+" << _chunkedBody.toString() << "+" << std::endl;
			this->status = 400;
			this->state = ERR;
			return false;
		}
		this->_body += _chunkedBody;
	}
	return true;
}