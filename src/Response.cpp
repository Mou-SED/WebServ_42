/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/23 10:38:37 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/06/12 20:12:12 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response( void )
{
	bytesSent = 0;
	return ;
}

Response::Response( uint16_t status )
{
	bytesSent = 0;
	this->_status = status;
	return ;
}

Response::~Response( void )
{
	return ;
}

std::string Response::getResponse(void) const
{
	return this->_response;
}

std::string Response::getDate(void)
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

UChar Response::openUri( std::string path )
{
	std::ifstream ifs(path.c_str());

    return UChar(
        std::istreambuf_iterator<char>(ifs),
        std::istreambuf_iterator<char>()
    );
}

void Response::toString(void)
{
	this->statusLineToStirng();
	this->headersToString();
	this->bodyToString();
}

void Response::statusLineToStirng(void)
{
	this->_response += "HTTP/1.1 ";
	this->_response += std::to_string(this->_status) + " " + getStatusMessage(this->_status);
	this->_response += "\r\n";
}

void Response::headersToString(void)
{
	this->_response += "Server: WebServ/1.0.0 (Unix)\r\n";
	this->_response += "Date: " + getDate() + "\r\n";
	this->_response += "Content-Type: " + this->_request.getContentType() + "\r\n";
	this->_response += "Content-Length: " + std::to_string(this->_body.size()) + "\r\n";
	this->_response += "Connection: close\r\n";
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

	if ( isDirectory(path) )
	{
		std::cerr << "Is Directory" << std::endl;
		// if ( this->_request.getServer()->isLocationExist() )
		// {
		// 	std::cerr << "Is Directory" << std::endl;
		// }
		// else this->_status = NOT_FOUND;
	}
	else if ( isFile(path) )
	{
		if ( access(path.c_str(), R_OK) == -1 )
			this->_status = FORBIDDEN;
		else
			this->_body = openUri(path);
	}
}

void	Response::clear(void)
{
	this->_response.clear();
}

void Response::setRequest(Request const &req)
{
	this->_request = req;
	this->_status = req.getStatus();
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
