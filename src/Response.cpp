/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/23 10:38:37 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/06/10 19:51:21 by moseddik         ###   ########.fr       */
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

Response::Response( Request const & req )
{
	bytesSent = 0;
	this->_request = req;
	this->_status = req.getStatus();
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

std::string openUri(std::string path)
{
	std::ifstream ifs(path.c_str());

    return std::string(
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
	this->_response += std::to_string(this->_status);
	this->_response += "\r\n";
}

void Response::headersToString(void)
{
	this->_response += "Server: WebServ/1.0.0 (Unix)\r\n";
	this->_response += "Date: " + getDate() + "\r\n";
	this->_response += "Content-Type: " + this->_request.getContentType() + "\r\n";
	this->_response += "Content-Length: " + this->_request.getContentLength() + "\r\n";
	this->_response += "Connection: close\r\n";
}

void Response::bodyToString(void)
{
	this->_response += "\r\n";
	std::string body = openUri(this->_request.getUri());
	this->_response.insert(this->_response.end(), body.begin(), body.end());
}

void Response::generateResponse(void)
{
	std::string method = this->_request.getMethod();
	std::cout << "method: " << method << std::endl;

	if ( method == "GET" )
	{
		std::cerr << "Hello from GET" << std::endl;
	}
	else if ( method == "POST" )
	{}
	else if ( method == "DELETE" )
	{}
}

void Response::generateStatusLine(void)
{
	
}

void	Response::clear(void)
{
	this->_response.clear();
}

void Response::setData(Server server, Request req)
{
	this->_server = server;
	this->_request = req;
}
