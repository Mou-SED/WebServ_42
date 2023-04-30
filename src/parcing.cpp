/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parcing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 22:23:43 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/04/30 20:38:05 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int  Check::num_line;
std::set<std::string> Check::methods;
std::string Check::key;
std::string Check::value;
std::string type_value[] = {"default_type","server","listen","server_name","error_page","client_max_body_size","location","root","index","client_body_temp_path","autoindex"};
std::stack<char> Check::brackets;



bool Check::ipAddress(std::string s)
{
	std::string buff("");
	std::vector<std::string> v;
	std::istringstream ss(s);
	while (getline(ss, buff, '.'))
	{
		if ( strspn(buff.c_str(), "0123456789") == buff.size() and buff != "" )
		{
			int x = atoi(buff.c_str());
			if (x < 0 || x > 255)
				return false;
			v.push_back(buff);
		}
		else
			return false;
	}
	if (v.size() != 4)
		return false;
	return true;
}

bool Check::isPort( std::string s )
{
	if ( strspn(s.c_str(), "0123456789") == s.size() and s != "" )
	{
		int x = atoi(s.c_str());
		if (x < 0 || x > 65535)
			return false;
	}
	else
		return false;
	return true;
}

std::string trim( std::string const & str )
{
	size_t first = str.find_first_not_of(" \t");
	if ( first == std::string::npos )
		return str;

	size_t last = str.find_last_not_of(" \t");
	return str.substr(first, (last - first + 1));
}

bool Check::isDomain( std::string const & s )
{
	if (s.size() > 255)
		throw std::runtime_error("Error: syntax error : `" + s + "` : too long");
	if (s.front() == '.' or s.back() == '.' or s.front() == '-' or s.back() == '-')
		return false;
	if ( strspn(s.c_str(), "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-.") == s.size() and s != "" )
		return true;
	return false;
}

void	check_value(std::string const &key, std::string const & value)
{
	if ( (key == "server" and value != "{") or (key == "location" and value != "{") )
		throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : server block must be followed by `{`");
	else if (key == "listen")
		listen_check(value);
	else if (key == "server_name")
		server_name_check(value);
	else if (key == "error_page")
		error_page_check(value);
	else if (key == "client_max_body_size")
		client_max_body_size_check(value);
	else if (key == "root" and value == ";")
		throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : missing `root` value");
	else if (key == "index" and value == ";")
		throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : missing `index` value");
	else if (key == "client_body_temp_path" and value == ";")
		throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : missing `client_body_temp_path` value");
	else if (key == "autoindex")
		autoindex_check(value);
	else if (key != "server" and key != "location")
		throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : unknown `" + key + "`");
}

std::string	check_syntax(std::ifstream &file)
{
	std::string line;
	std::string key, value;
	std::string token;

	for (size_t i = 0; i < sizeof(type_value) / sizeof(type_value[0]); i++)
		Check::methods.insert(type_value[i]);
	while (getline(file, line))
	{
		std::stringstream ss;
		ss << line;
		ss >> key;
		ss >> value;
		if ( (line.find(";") == std::string::npos or line.find(";") != line.size() - 1)
				and (key != "server" and key != "location") )
			throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : missing `;`");
		value = line.substr(line.find(value) - 1);// TODO :[A] check if value is empty of equal to ";"
		if (Check::methods.find(key) == Check::methods.end())
			throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : unknown `" + key + "`");
		else
			check_value(key, trim(value));
		token += key + value + " ";
		Check::num_line++;
	}
	return token;
}

bool parcing(std::ifstream &file, std::string s)
{
	if (not file.is_open())
		return (false);
	(void)s;
	check_syntax(file);
	return (true);
}