/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/23 10:38:37 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/06/18 17:13:59 by moseddik         ###   ########.fr       */
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
	if (this->_Location != "")
	{
		this->_headers += "Location: " + this->_Location + "\r\n";
		this->_headers += "Connection: close\r\n\r\n";
		return ;
	}
	this->_headers += "Content-Type: " + type + "\r\n";
	this->_headers += "Content-Length: " + std::to_string(this->_bodySize) + "\r\n";
	this->_headers += "Connection: close\r\n\r\n";
	// TODO : Add headers for cookies and redirection.
}

void Response::toStringGet( void )
{
	this->_headers += "HTTP/1.1 ";
	this->_headers += std::to_string(this->_status) + " " + getStatusMessage(this->_status);
	this->_headers += "\r\n";
	this->_headers += "Server: WebServ/1.0.0 (Unix)\r\n";
	this->_headers += "Date: " + getDate() + "\r\n";
	if (this->_Location != "")
	{
		this->_headers += "Location: " + this->_Location + "\r\n";
		this->_headers += "Connection: close\r\n\r\n";
		return ;
	}
	this->_headers += "Accept-Ranges: bytes\r\n";
	this->_headers += "Content-Type: " + this->_request.getContentType() + "\r\n";
	this->_headers += "Content-Length: " + std::to_string(this->_bodySize) + "\r\n";
	this->_headers += "Connection: close\r\n\r\n";
	// TODO : Add headers for cookies and redirection.
}

void Response::toStringDelete( void )
{
	this->_headers += "HTTP/1.1 ";
	this->_headers += std::to_string(this->_status) + " " + getStatusMessage(this->_status);
	this->_headers += "\r\n";
	this->_headers += "Server: WebServ/1.0.0 (Unix)\r\n";
	this->_headers += "Date: " + getDate() + "\r\n";
	this->_headers += "Connection: close\r\n\r\n";
}

void Response::toStringPut( void )
{
	this->_headers += "HTTP/1.1 ";
	this->_headers += std::to_string(this->_status) + " " + getStatusMessage(this->_status);
	this->_headers += "\r\n";
	this->_headers += "Server: WebServ/1.0.0 (Unix)\r\n";
	this->_headers += "Date: " + getDate() + "\r\n";
	this->_headers += "Content-Location: " + this->_request.getUri() + "\r\n";
	this->_headers += "Connection: close\r\n\r\n";
}

void Response::generateResponse(void)
{
	std::string method = this->_request.getMethod();

	std::string uri = this->_request.getUri();
	this->setUri(uri);
	std::string root = this->_request.getServer()->getRoot();


	std::pair<std::string, Directives > * location = this->_request.getServer()->matchLocation(uri);
	if ( location != nullptr and location->second.count("root") != 0 ) root = location->second["root"][0];

	std::string path = this->_request.getServer()->getRoot() + uri;
	this->setPath(path);

	if (findVector(this->_request.getMethod(), location->second["allowed_methods"]) == false)
	{
		this->_status = METHOD_NOT_ALLOWED;
		this->generateErrorResponse();
		return ;
	}

	if ( method == "GET" ) GET( location );
	else if ( method == "PUT" ) PUT();
	else if ( method == "POST" ) POST( location );
	else if ( method == "DELETE" ) DELETE();
}

bool	Response::redirection(std::string &path,std::string const & uri, std::pair<std::string, Directives > * location)
{
	std::vector<std::string> ret = split(location->second["return"][0], ' ', false);
	this->_status = atoi(ret[0].c_str());
	if (ret.size() != 2)
	{
		this->_status = INTERNAL_SERVER_ERROR;
		return true;
	}
	else if (ret[1][0] != '/' and ret[1][0] != '.')
	{
		this->_Location = ret[1];
		return true;
	}
	else
	{
		path = this->_request.getServer()->getRoot() + ret[1] + uri;
		return false;
	}
}

void Response::GET( std::pair<std::string, Directives > * location )
{
	if ( not this->_request.getServer()->isLocationExist() )
	{
		this->_status = NOT_FOUND;
		goto GENERATE;
	}
	if (location->second.find("return") != location->second.end() and redirection(this->_path, this->getUri(), location))
		goto GENERATE;
	if ( isDirectory(this->_path) )
	{
		if ((location->second.find("autoindex") != location->second.end() and location->second["autoindex"][0] == "on" and  checkDirFile(this->_path, "index.html")) or (location->second.find("index") != location->second.end() and checkDirFile(this->_path, location->second["index"][0])))
		{
			this->setBodySize(this->_path);
		}
		else
		{
			this->_request.getServer()->setRootDirectory(this->_path);
			std::string const _dir = generateDirectory(this->_path);
			this->setBodySize(_dir.length());
			this->_buffer = strdup(_dir.c_str());
			this->_isDir = true;
		}
		toString("text/html");
		return ;
	}
	else
	{
		if ( access(this->_path.c_str(), F_OK) == -1 )
			this->_status = NOT_FOUND;
		else if ( access(this->_path.c_str(), R_OK) == -1 )
			this->_status = FORBIDDEN;
		else
		{
			this->setBodySize(this->_path);
		}
	}
	GENERATE:
		if ( this->_status >= BAD_REQUEST)
			this->generateErrorResponse();
		else
			this->toStringGet();
}

void Response::DELETE( void )
{
	if ( isDirectory(this->_path) or access(this->_path.c_str(), F_OK) == -1 )
		this->_status = FORBIDDEN;
	else if ( access(this->_path.c_str(), W_OK) == -1 )
		this->_status = FORBIDDEN;
	else
	{
		if ( not remove(this->_path.c_str()) )
			this->_status = INTERNAL_SERVER_ERROR;
		this->_status = NO_CONTENT;
	}
	this->_buffer = strdup("");
	if ( this->_status >= BAD_REQUEST )
		this->generateErrorResponse();
	else
		this->toStringDelete();
}

void Response::PUT( void )
{
	std::ifstream ifs(this->_path.c_str());
	if ( ifs )
	{
		ifs.close();
		std::ofstream ofs(this->_path.c_str());
		ofs << this->_request.getBody();
		ofs.close();
		this->_status = NO_CONTENT;
	}
	else
	{
		std::ofstream ofs(this->_path.c_str());
		ofs << this->_request.getBody();
		ofs.close();

		this->_status = CREATED;
	}
	if ( isDirectory(this->_path.c_str()) )
		this->_status = CONFLICT;

	this->_buffer = strdup("");
	if ( this->_status >= BAD_REQUEST )
		this->generateErrorResponse();
	else
		this->toStringPut();
}

void Response::POST( std::pair<std::string, Directives > * location )
{
	// TODO : Handle CGI
	// if file is CGI run it
	// else
	GET( location );
}

void Response::generateErrorResponse( void )
{
	Error error(this->_status, this->_error_pages);
	this->setBodySize(error.getErrorBody().length());
	this->_buffer = strdup(error.getErrorBody().c_str());
	this->toString("text/html");
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

void Response::setUri(std::string &uri)
{
	this->_uri = uri;
}

uint16_t Response::getStatus(void) const
{
	return this->_status;
}

std::string Response::getUri(void) const
{
	return this->_uri;
}
