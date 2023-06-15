/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/23 10:38:37 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/06/15 20:05:49 by aaggoujj         ###   ########.fr       */
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

bool	checkDirFile(std::string &path, std::string const &file)
{
	DIR *dir;
	struct dirent *ent;
	bool ret = false;

	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			std::cerr << ent->d_name << std::endl;
			if ( ent->d_name == file )
			{
				ret = true;
				break ;
			}
		}
		closedir(dir);
	}
	return ret;
}

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <ctime>

std::string const generateDirectory(std::string const &path)
{
    std::stringstream ss;
    ss << "<!DOCTYPE html>\n<html>\n\t<head>\n\t\t<title>Directory Listing</title>\n\t\t<style>\n\t\t\ttable {\n\t\t\t\tborder-collapse: collapse;\n\t\t\t\tmargin: auto;\n\t\t\t\twidth: 100%;\n\t\t\t}\n\t\t\tth, td {\n\t\t\t\tpadding: 8px;\n\t\t\t\ttext-align: left;\n\t\t\t\tborder-bottom: 1px solid #ddd;\n\t\t\t}\n\t\t\tth {\n\t\t\t\tbackground-color: #f2f2f2;\n\t\t\t}\n\t\t</style>\n\t</head>\n\t<body>\n\t\t<h1>Directory Listing</h1>\n\t\t<hr>\n\t\t<table>\n\t\t\t<tr><th>Type</th><th>Name</th><th>Size (bytes)</th><th>Creation Date</th><th>Extension</th></tr>\n";

    DIR* dir = opendir(path.c_str());
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] == '.') {
                continue;
            }
            struct stat fileStat;
            std::string entryPath = path + "/" + entry->d_name;
            if (stat(entryPath.c_str(), &fileStat) < 0) {
                std::cerr << "Error: Failed to get file stats for " << entryPath << std::endl;
                continue;
            }
            ss << "\t\t\t<tr>\n";
            if (S_ISDIR(fileStat.st_mode)) {
                ss << "\t\t\t\t<td>Directory</td>\n";
                ss << "\t\t\t\t<td><a href=\"" << entry->d_name << "/\">" << entry->d_name << "/</a></td>\n";
                ss << "\t\t\t\t<td></td>\n";
                ss << "\t\t\t\t<td>" << std::put_time(std::gmtime(&fileStat.st_ctime), "%Y-%m-%d %H:%M:%S") << "</td>\n";
                ss << "\t\t\t\t<td></td>\n";
            } else if (S_ISREG(fileStat.st_mode)) {
                ss << "\t\t\t\t<td>File</td>\n";
                ss << "\t\t\t\t<td><a href=\"" << entry->d_name << "\">" << entry->d_name << "</a></td>\n";
                ss << "\t\t\t\t<td>" << fileStat.st_size << "</td>\n";
                ss << "\t\t\t\t<td>" << std::put_time(std::gmtime(&fileStat.st_ctime), "%Y-%m-%d %H:%M:%S") << "</td>\n";
                ss << "\t\t\t\t<td>" << std::string(entry->d_name).substr(std::string(entry->d_name).find_last_of(".") + 1) << "</td>\n";
            }
            ss << "\t\t\t</tr>\n";
        }
        closedir(dir);
    } else {
        std::cerr << "Error: Failed to open directory " << path << std::endl;
    }

    ss << "\t\t</table>\n";
    ss << "\t</body>\n";
    ss << "</html>\n";
    return ss.str();
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
		if ((location->second.find("autoindex") != location->second.end() and location->second["autoindex"][0] == "on" and  checkDirFile(path, "index.html")) or (location->second.find("index") != location->second.end() and checkDirFile(path, location->second["index"][0])))
		{
			std::cerr << "Autoindex on" << std::endl;
			path += "/index.html";
			this->setPath(path);
			this->setBodySize(path);
		}
		else
		{
			std::string const _dir = generateDirectory(path);
			std::cerr << "Dir :\n" << _dir << std::endl;
			this->setBodySize(_dir.length());
			this->_buffer = strdup(_dir.c_str());
			this->_isDir = true;
		}
		toString("text/html");
		return ;
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
