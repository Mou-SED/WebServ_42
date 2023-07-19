/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/23 10:38:37 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/07/19 10:30:59 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Response.hpp"

Response::Response( void )
{
	_isCGI = false;
	_isDir = false;
	bytesSent = 0;
	_bodySize = 0;
	return ;
}

Response::Response( uint16_t status )
{
	_isCGI = false;
	_isDir = false;
	bytesSent = 0;
	_bodySize = 0;
	this->_status = status;
	return ;
}

Response::~Response( void )
{
	if (_bodySize > 0)
	{
		this->_bodySize = 0;
		delete [] _buffer;
	}
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
	if (this->_status >= MOVED_PERMANENTLY and this->_status <= TEMPORARY_REDIRECT)
	{
		if (this->_Location != "")
			this->_headers += "Location: " + this->_Location + "\r\n";
		else
			this->_headers += "Location: " + this->getPath() + "\r\n";
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

bool	checking_extension(std::string const &Path, std::string const & cgi)
{
	std::string ext;
	std::string extension = cgi.substr(0, cgi.find(" "));
	if (Path.find("?") != std::string::npos)
		ext = Path.substr(Path.find(".") + 1, Path.find("?") - Path.find(".") - 1);
	else
	 	ext = Path.substr(Path.find(".") + 1);
	return (ext == extension);
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

	if (location->second.find("allowed_methods") != location->second.end() and findVector(this->_request.getMethod(), location->second["allowed_methods"]) == false)
	{
		this->_status = METHOD_NOT_ALLOWED;
		this->generateErrorResponse();
		return ;
	}
	
	if ( method == "POST" or checking_extension(getUri(), location->second["cgi_pass"][0]) ) POST( location );
	else if ( method == "GET" ) GET( location );
	else if ( method == "PUT" ) PUT();
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
		this->setPath(ret[1]);
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
	if (location->second.find("return") != location->second.end() and not redirection(this->_path, this->getUri(), location))
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
	if ( isDirectory( this->_path ) )
	{
		removeDirectory(this->_path.c_str());
		this->_status = NO_CONTENT;
		goto END;
	}
	if ( access(this->_path.c_str(), F_OK) == -1 )
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
	END:
		if ( this->_status >= BAD_REQUEST )
			this->generateErrorResponse();
		else
			this->toStringDelete();
}

void Response::PUT( void )
{
	char c = this->_request.getServer()->directives["client_max_body_size"][0].back();
	size_t max_body_size = std::atoi(this->_request.getServer()->directives["client_max_body_size"][0].c_str());
	if ( c == 'K' or c == 'k' )
		max_body_size *= 1024;
	else if ( c == 'M' or c == 'm' )
		max_body_size *= MYBUFSIZ;
	if ( this->_request._bodySize > (off_t)max_body_size )
	{
		this->_status = REQUEST_ENTITY_TOO_LARGE;
		this->generateErrorResponse();
		return ;
	}

	this->_status = CREATED;
	std::ifstream ifs(this->_path.c_str());
	if ( ifs.is_open() )
	{
		this->_status = NO_CONTENT;
		ifs.close();
	}
	if (access(this->_path.c_str(), F_OK) == 0)
		this->_status = NO_CONTENT;
	int fd = open(this->_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd < 0)
	{
		this->_status = INTERNAL_SERVER_ERROR;
		this->generateErrorResponse();
		return ;
	}

	off_t len = this->_request._bodySize;
	char buffer[MYBUFSIZ + 1] = {0};
	while ( len > 0 and this->_request.file.WR_fd != -1 )
	{
		int readBytes = read(this->_request.file.WR_fd, buffer, MYBUFSIZ);
		write(fd, buffer, readBytes);
		len -= readBytes;
	}
	close(fd);

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
	this->_isCGI = true;
	std::string Methode;
	if (this->getUri().find("?") == std::string::npos and this->_request.getMethod() == "POST")
		Methode = "POST";
	else
		Methode = "GET";
	Cgi cgi(this->getUri(), Methode, this->_request, location);
	if (cgi.getStatus() >= BAD_REQUEST)
	{
		this->_status = cgi.getStatus();
		this->generateErrorResponse();
		return ;
	}
	cgi.execute();
	this->_status = cgi.getStatus();
	if (this->_status >= BAD_REQUEST)
	{
		this->generateErrorResponse();
		return ;
	}
	this->_bodySize = cgi.getSizeBody();
	this->_buffer = strdup(cgi.getBody().c_str());
	this->_headers += "HTTP/1.1 ";
	this->_headers += std::to_string(this->_status) + " " + getStatusMessage(this->_status);
	this->_headers += "\r\n";
	this->_headers += "Server: WebServ/1.0.0 (Unix)\r\n";
	this->_headers += "Date: " + getDate() + "\r\n";
	this->_headers += "Content-Type: " + cgi.getContentType() + "\r\n";//generateContentType(this->_path) + "\r\n";
	if (cgi.getLocation() != "")
		this->_headers += "Location: " + cgi.getLocation() + "\r\n";
	if (cgi.getCookie() != "")
		this->_headers += "set-cookie: " + cgi.getCookie() + "\r\n";
	this->_headers += "Content-Length: " + std::to_string(this->_bodySize) + "\r\n";
	this->_headers += "Connection: close\r\n\r\n";
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
	if (this->_bodySize > 0)
	{
		this->_bodySize = 0;
		delete [] this->_buffer;
	}

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
