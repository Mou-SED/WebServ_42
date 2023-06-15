/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/23 10:38:37 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/06/15 17:48:10 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response( void )
{
	bytesSent = 0;
	_bodySize = 0;
	return ;
}

Response::Response( uint16_t status )
{
	bytesSent = 0;
	_bodySize = 0;
	this->_status = status;
	return ;
}

Response::~Response( void )
{
	delete [] this->_buffer;
	return ;
}

std::string Response::getResponse(void) const
{
	return this->_response;
}

std::string Response::getDate(void) const
{
	time_t rawTime;
	struct tm * timeInfo;
	char buffer[80];

	time (&rawTime);
	timeInfo = localtime(&rawTime);

	strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S %Z", timeInfo);
	std::string str(buffer);

	return str;
}

void Response::toString( std::string const  &type)
{
	this->_headers += "HTTP/1.1 ";
	this->_headers += std::to_string(this->_status) + " " + getStatusMessage(this->_status);
	this->_headers += "\r\n";
	this->_headers += "Server: WebServ/1.0.0 (Unix)\r\n";
	this->_headers += "Date: " + getDate() + "\r\n";
	this->_headers += "Content-Type: " + type + "\r\n";
	this->_headers += "Content-Length: " + std::to_string(this->_bodySize) + "\r\n";
	this->_headers += "Connection: close\r\n\r\n";
	// TODO : Add headers for cookies and redirection.
}

void Response::toString( void )
{
	this->_headers += "HTTP/1.1 ";
	this->_headers += std::to_string(this->_status) + " " + getStatusMessage(this->_status);
	this->_headers += "\r\n";
	this->_headers += "Server: WebServ/1.0.0 (Unix)\r\n";
	this->_headers += "Date: " + getDate() + "\r\n";
	this->_headers += "Content-Type: " + this->_request.getContentType() + "\r\n";
	this->_headers += "Content-Length: " + std::to_string(this->_bodySize) + "\r\n";
	this->_headers += "Connection: close\r\n\r\n";
	// TODO : Add headers for cookies and redirection.
}

void Response::bodyToString(void)
{
	this->_response += "\r\n";

	std::vector<unsigned char> & body = this->_body.getBuffer();
	this->_response.insert(this->_response.end(), body.begin(), body.end());
}

void Response::generateResponse(void)
{
	std::string method = this->_request.getMethod();

	if ( method == "GET" )
	{
		GET();
	}
	else if ( method == "POST" )
	{}
	else if ( method == "DELETE" )
	{}
}

void Response::GET( void )
{
	std::string uri = this->_request.getUri();
	std::string root = this->_request.getServer()->getRoot();

	std::pair<std::string, Directives > * location = this->_request.getServer()->matchLocation(uri);
	if ( location != nullptr and location->second.count("root") != 0 ) root = location->second["root"][0];

	std::string path = this->_request.getServer()->getRoot() + uri;

	if ( not this->_request.getServer()->isLocationExist() )
	{
		this->_status = NOT_FOUND;
		goto GENERATE;
	}

	if ( isDirectory(path) )
	{
		std::cerr << "Is Directory" << std::endl;
		// if ( this->_request.getServer()->isLocationExist() )
		// {
		// 	std::cerr << "Is Directory" << std::endl;
		// }
		// else this->_status = NOT_FOUND;
	}
	else
	{
		if ( access(path.c_str(), F_OK) == -1 )
			this->_status = NOT_FOUND;
		else if ( access(path.c_str(), R_OK) == -1 )
			this->_status = FORBIDDEN;
		else
		{
			this->setPath(path);
			this->setBodySize(path);
		}
	}
	GENERATE:
		if ( this->_status >= BAD_REQUEST )
		{
			Error error(this->_status, this->_error_pages);
			this->setBodySize(error.getErrorBody().length());
			this->_buffer = strdup(error.getErrorBody().c_str());
			this->toString("text/html");
		}
		else
			this->toString();
}

std::string Response::getHeaders(void) const
{
	return this->_headers;
}

std::string Response::getPath(void) const
{
	return this->_path;
}

off_t Response::getBodySize(void) const
{
	return this->_bodySize;
}

void Response::clear(void)
{
	this->_response.clear();
}

std::pair<std::set<int>, std::string> getErrorPage(std::string &error_page)
{
	std::set<int> status_codes;
	std::string path;

	std::vector<std::string> tokens = split(error_page, ' ', false);
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if ( tokens[i].find("/") == std::string::npos)
			status_codes.insert(atoi(tokens[i].c_str()));
		else
			path = tokens[i];
	}
	return std::make_pair(status_codes, path);
}

void Response::setErrorPages(std::vector<std::string> &error_pages)
{
	for (size_t i = 0; i < error_pages.size(); i++)
	{
		this->_error_pages.push_back(getErrorPage(error_pages[i]));
	}
}

void Response::setRequest(Request const &req)
{
	this->_request = req;
	this->_status = req.getStatus();
}

void Response::setBodySize(std::string &path)
{
	struct stat st;
	stat(path.c_str(), &st);
	this->_bodySize = st.st_size;
}

void Response::setBodySize( off_t size )
{
	this->_bodySize = size;
}

void Response::setPath(std::string &path)
{
	this->_path = path;
}

std::string Response::getStatusMessage(uint16_t status)
{
	switch(status)
	{
		case OK:
			return "OK";
		case CREATED:
			return "Created";
		case NO_CONTENT:
			return "No Content";
		case MOVED_PERMANENTLY:
			return "Moved Permanently";
		case MOVED_TEMPORARILY:
			return "Moved Temporarily";
		case BAD_REQUEST:
			return "Bad Request";
		case FORBIDDEN:
			return "Forbidden";
		case NOT_FOUND:
			return "Not Found";
		case INTERNAL_SERVER_ERROR:
			return "Internal Server Error";
		case NOT_IMPLEMENTED:
			return "Not Implemented";
		case BAD_GATEWAY:
			return "Bad Gateway";
		default:
			throw std::runtime_error("Unknown status code");
	}
}

uint16_t Response::getStatus(void) const
{
	return this->_status;
}
