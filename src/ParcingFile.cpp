/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParcingFile.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 22:23:43 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/07/20 11:05:55 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/WebServ.hpp"
#include "../include/Tokenization.hpp"

int  Check::num_line;
std::set<std::string> Check::methods;
std::string Check::key;
std::string Check::value;
std::string arr[] = {"default_type", "server", "listen", "server_name", "error_page", "client_max_body_size", "location", "root", "index", "autoindex", "return", "cgi_pass", "allowed_methods"};
std::vector<std::string> type_value(arr, arr + sizeof(arr) / sizeof(arr[0]));
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

std::string trim( std::string const & str)
{
	size_t first = str.find_first_not_of(" \t");
	if ( first == std::string::npos )
		return str;

	size_t last = str.find_last_not_of(" \t");
	return str.substr(first, (last - first + 1));
}

std::string trim( std::string const & str, std::string const & whitespace)
{
	size_t first = str.find_first_not_of(whitespace);
	if ( first == std::string::npos )
		return str;

	size_t last = str.find_last_not_of(whitespace);
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

void	cgi_pass_check(std::string const & value)
{
	if (value == "" or value == ";")
		throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : missing `cgi_pass` value");
}

void	check_value(std::string const &key, std::string const & value)
{
	if ( (key == "server" and value != "{") )
		throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : server block must be followed by `{`");
	else if (key == "location")
		location_check(value);
	else if (key == "listen")
		listen_check(value);
	else if (key == "server_name")
		server_name_check(value);
	else if (key == "error_page")
		error_page_check(value);
	else if (key == "allowed_methods")
		allowed_methods_check(value);
	else if (key == "client_max_body_size")
		client_max_body_size_check(value);
	else if (key == "root")
		root_check(value);
	else if (key == "index")
		index_check(value);
	else if (key == "cgi_pass")
		cgi_pass_check(value);
	else if (key == "autoindex")
		autoindex_check(value);
	else if (key == "return")
		redirectionCheck(value);
	else if (key != "server" and key != "location")
		throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : unknown `" + key + "`");
}

void allowed_methods_check(std::string const & value)
{
	std::vector<std::string> methods = split(value, ' ', false);

	for (size_t i = 0; i < methods.size() - 1; i++)
	{
		if (methods[i] == "GET" or methods[i] == "POST" or methods[i] == "DELETE" or methods[i] == "PUT")
			continue ;
		else
			throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : unknown method `" + methods[i] + "`");
	}
}

std::string  get_value(std::string const &str, size_t pos)
{
	std::string value = "";
	for (size_t i = pos + 1; i < str.size() and str[i] != ';'; i++)
			value.push_back(str[i]);
	return trim(value);
}

void	check_syntax(std::ifstream &file)
{
	std::string line;
	std::string token;

	Check::methods.insert(type_value.begin(), type_value.end());
	while (getline(file, line))
	{
		std::replace(line.begin(), line.end(), '\t', ' ');
		line = trim(line);
		Check::num_line++;
		std::string key, value;
		std::stringstream ss(line);
		ss >> key;
		value = get_value(line, key.size());
		if (key[0] == '#' or key == "")
			continue ;
		else if (key == "}" and !Check::brackets.empty())
			Check::brackets.pop();
		else if (key == "}" and Check::brackets.empty())
			throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : missing `{`");
		else
		{
			if (key == "server" and value == "{")
				Check::brackets.push('{');
			if ( (line.find(";") == std::string::npos or line.find(";") != line.size() - 1)
					and (key != "server" and key != "location" and key != "}") )
				throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error");
			value = trim(line.substr(line.find(value) - 1));
			if (key != "}" and std::count(value.begin(), value.end(), ';') > 1)
				throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error");
			if (Check::methods.find(key) == Check::methods.end())
				throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : unknown `" + key + "`");
			else
				check_value(key, value);
		}
	}
	file.close();
	if (Check::brackets.size() != 0)
		throw std::runtime_error("Line : " + std::to_string(Check::num_line) + " : syntax error : missing close bracket `}`");
}

void	print_directivers(std::vector<std::string> directives)
{
	for (size_t i = 0; i < directives.size(); i++)
		std::cout << directives[i] << " ";
}

void	print_context(std::vector<std::string> context)
{
	for (size_t i = 0; i < context.size(); i++)
		std::cout << context[i] << " ";
}

bool parcing(std::ifstream &file, std::string s)
{
	if (not file.is_open())
		return (false);
	(void)s;
	type_value[0] = "default_type";type_value[1] = "server";type_value[2] = "listen";type_value[3] = "server_name";
	type_value[4] = "error_page";type_value[5] = "client_max_body_size";type_value[6] = "location";type_value[7] = "root";type_value[8] = "index";
	type_value[9] = "autoindex";type_value[10] = "return";type_value[11] = "cgi_pass";type_value[12] = "allowed_methods";
	check_syntax(file);
	return (true);
}