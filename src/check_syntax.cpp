/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_syntax.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/29 18:56:18 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/04/30 15:14:35 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"


void	listen_check(std::string const & value, const int i)
{
	std::string tmp;
	std::string trim_value = trim(value);

	if (value.find(':') != std::string::npos)
	{
		tmp = trim_value.substr(0, trim_value.find(':'));
		if (!ipAddress(tmp))
			throw std::runtime_error("Line : " + std::to_string(i) + " : syntax error : invalid ip address");
		tmp = trim_value.substr(trim_value.find(':') + 1, trim_value.find(';') - trim_value.find(':') - 1);
		if (!isPort(tmp))
			throw std::runtime_error("Line : " + std::to_string(i) + " : syntax error : invalid port number");
		int x = atoi(tmp.c_str());
		if (x < 0 || x > 65535)
			throw std::runtime_error("Line : " + std::to_string(i) + " : syntax error : invalid port number");
	}
	else
	{
		tmp = trim_value.substr(0, trim_value.find(';'));
		if ( trim_value.find('.') != std::string::npos )
		{
			if (!ipAddress(tmp))
				throw std::runtime_error("Line : " + std::to_string(i) + " : syntax error : invalid ip address");
		}
		else
		{
			if (!isPort(tmp))
				throw std::runtime_error("Line : " + std::to_string(i) + " : syntax error : invalid port number");
			int x = atoi(tmp.c_str());
			if (x < 0 || x > 65535)
				throw std::runtime_error("Line : " + std::to_string(i) + " : syntax error : invalid port number");
		}
	}
}

void	server_name_check(std::string const & value)
{
	(void)value;
}

void	error_page_check(std::string const & value)
{
	std::stringstream ss(value);
	std::string s;
	while (ss >> s)
	{
		if (strspn(s.c_str(), "0123456789") == s.size() and (atoi(s.c_str()) < 300 or atoi(s.c_str()) > 599) )
			throw std::runtime_error("Error: syntax error : `" + s + "`");
		if (strspn(s.c_str(), "0123456789") != s.size() and s.back() != ';')
			throw std::runtime_error("Error: syntax error code : " + s);
	}
}

void	client_max_body_size_check(std::string const & value)
{
	std::stringstream ss(value);
	std::string s;
	ss >> s;
	int num = atoi(s.c_str());
	s.erase(0, std::to_string(num).size());
	s.erase(s.size() - 1, 1);
	if (num < 0 or num > 1024 or (s != "M" and s != "K" and s != ""))
		throw std::runtime_error("Error: syntax error code :" + value.substr(0, value.size() - 1));
}

void	autoindex_check(std::string const & value)
{
	std::string s(value);
	if (s.back() != ';')
		throw std::runtime_error("Error: syntax error : missing `;`");
	s = s.substr(strspn(" \t", s.c_str()));
	s.erase(s.size() - 1, 1);
	if (s != "on" and s != "off")
		throw std::runtime_error("Error: syntax error : missing `on` or `off`");
}
