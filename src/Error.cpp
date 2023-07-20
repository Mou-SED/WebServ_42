/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/17 06:46:52 by junik             #+#    #+#             */
/*   Updated: 2023/07/20 08:15:54 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Error.hpp"

Error::Error( uint16_t error )
{
	this->_error = error;
	this->_errorPage = "";
	setErrorMessage(getStatusMessage(error));
	return;
}

std::pair<bool, std::string>	errorPageFound(uint16_t error, std::vector<std::pair<std::set<int>, std::string> > error_page)
{
	for (size_t i = 0; i < error_page.size(); i++)
	{
		if (error_page[i].first.find(error) != error_page[i].first.end())
			return std::make_pair(true, error_page[i].second);
	}
	return std::make_pair(false, "");
}

Error::Error(uint16_t error, std::vector<std::pair<std::set<int>, std::string> > error_page)
{
	this->_error = error;
	std::pair<bool, std::string> error_page_found = errorPageFound(error, error_page);
	if (error_page_found.first and access(error_page_found.second.c_str(), R_OK) != -1)
	{
		std::ifstream file(error_page_found.second.c_str());
		this->_errorBody = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	}
	else
		setErrorMessage(getStatusMessage(error));
	return ;
}

Error::Error(uint16_t error, std::string error_page)
{
	this->_error = error;
	this->_errorPage = error_page;
	return;
}

Error::Error(const Error &src)
{
	this->_error = src._error;
	this->_errorMessage = src._errorMessage;
	this->_errorBody = src._errorBody;
	this->_errorPage = src._errorPage;
	return;
}

Error::~Error()
{
	return;
}

void 	Error::setErrorMessage( std::string const & msg )
{
	std::string s = "<html>\n<head><title>" + msg + "</title></head>\n<body>\n<center><h1>" + msg + "</h1></center>\n<hr><center>WebServ/0.1</center>\n</body>\n</html>\n";
	this->_errorBody = s;
	std::string header  = "HTTP/1.1 " + std::to_string(this->_error) + " " + msg + CRLF + "Content-Type: text/html" + CRLF + "Content-Length: " + std::to_string(s.length()) + CRLF + CRLF;
	this->_errorMessage = header + s;
}

std::string Error::getErrorMessage(void) const
{
	return this->_errorMessage;
}

std::string Error::getErrorBody(void) const
{
	return  _errorBody;
}
