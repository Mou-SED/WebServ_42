/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/23 10:38:37 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/06/09 11:59:19 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(void)
{
	bytesSent = 0;
	return ;
}

Response::Response(std::string status)
{
	bytesSent = 0;
	this->_status = status;
	return ;
}

Response::Response(Response const &src)
{
	bytesSent = 0;
	*this = src;
	return ;
}

Response::Response(Server server, Request req)
{
	bytesSent = 0;
	this->_server = server;
	this->_req = req;
	return ;
}

Response::~Response(void)
{
	return ;
}

std::string Response::getResponse(void) const
{
	return this->_response;
}

std::string Response::getDate(void)
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S %Z", timeinfo);
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

void Response::generateResponse(void)
{
	this->generateStatusLine();
	this->generateHeaders();
	this->generateBody();
}

void Response::generateStatusLine(void)
{
	this->_response += "HTTP/1.1 ";
	this->_response += this->_status;
	this->_response += "\r\n";
}

void Response::generateHeaders(void)
{
	this->_response += "Server: WebServ/1.0.0 (Unix)\r\n";
	this->_response += "Date: " + getDate() + "\r\n";
	this->_response += "Content-Type: " + this->_req.getContentType() + "\r\n";
	this->_response += "Content-Length: " + this->_req.getContentLength() + "\r\n";
	this->_response += "Connection: close\r\n";
}

void Response::generateBody(void)
{
	this->_response += "\r\n";
	std::string body = openUri(this->_req.getUri());
	this->_response.insert(this->_response.end(), body.begin(), body.end());
}

void	Response::clear(void)
{
	this->_response.clear();
}

void Response::setData(Server server, Request req)
{
	this->_server = server;
	this->_req = req;
}
