/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_syntax.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/29 18:56:18 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/05/01 13:27:23 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"


void	listen_check(std::string const & value)
{
	std::string tmp;

	if (value.find(':') != std::string::npos)
	{
		tmp = value.substr(0, value.find(':'));
		if (!Check::ipAddress(tmp) and !Check::isDomain(tmp))
			throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : invalid ip address or domain name");
		tmp = value.substr(value.find(':') + 1, value.find(';') - value.find(':') - 1);
		if (!Check::isPort(tmp))
			throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : invalid port number");
		int x = atoi(tmp.c_str());
		if (x < 0 || x > 65535)
			throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : invalid port number");
	}
	else
	{
		tmp = value.substr(0, value.find(';'));
		if ( value.find('.') != std::string::npos )
		{
			if (!Check::ipAddress(tmp))
				throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : invalid ip address");
		}
		else
		{
			if (!Check::isPort(tmp))
				throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : invalid port number");
			int x = atoi(tmp.c_str());
			if (x < 0 || x > 65535)
				throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : invalid port number");
		}
	}
}

void	server_name_check(std::string const & value)
{
	std::string s(value);
	s.erase(s.size()- 1, 1);
	std::stringstream ss(s);
	while (ss >> s)
	{
		s = trim(s);
		if (s.front() == '-' or s.back() == '-')
			throw std::runtime_error("Error: syntax error : `" + s + "`");
		if (s.size() > 255)
			throw std::runtime_error("Error: syntax error : `" + s + "` : too long");
		for (size_t i = 0; i < s.size() ;i++)
		{
			if (!isalnum(s[i]) and s[i] != '.' and s[i] != '-')
				throw std::runtime_error("Error: syntax error : `" + s + "`");
		}
	}
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

void	root_check(std::string const & value)
{
	if (value == ";")
		throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : missing `root` value");
}

void	index_check(std::string const & value)
{
	if (value == ";")
		throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : missing `index` value");
}

void	location_check(std::string const & value)
{
	std::string s(value);
	if (s.back() != '{')
		throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : missing `{`");
	if (s.size() < 2)
		throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : missing `location` value");
	Check::brackets.push('{');
}
