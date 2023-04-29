/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parcing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 22:23:43 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/04/29 18:59:12 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

bool ipAddress(std::string s)
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

void	check_value(std::string const &key, std::string const & value, const int i)
{
	if (key == "server") // TODO : [M] first tree statement
		server_check(value);
	else if (key == "listen")
		listen_check(value);
	else if (key == "server_name")
		server_name_check(value);
	else if (key == "error_page") // TODO : [A] last tree statement
		error_page_check(value);
	else if (key == "client_max_body_size")
		client_max_body_size_check(value);
	else if (key == "root" and value == ";")
		throw std::runtime_error("Line : " + std::to_string(i) + " : syntax error : missing `root` value");
	else if (key == "index" and value == ";")
		throw std::runtime_error("Line : " + std::to_string(i) + " : syntax error : missing `index` value");
	else if (key == "client_body_temp_path" and value == ";")
		throw std::runtime_error("Line : " + std::to_string(i) + " : syntax error : missing `client_body_temp_path` value");
	else if (key == "autoindex")
		autoindex_check(value);
	else
		throw std::runtime_error("Line : " + std::to_string(i) + " : syntax error : unknown `" + key + "`");
}

std::string	check_syntax(std::ifstream &file)
{
	std::string line;
	std::string key, value;
	std::string token; 
	std::string arr[] = {"default_type","server","listen","server_name","error_page","client_max_body_size","location","root","index","client_body_temp_path","autoindex"};
	std::set<std::string> lt(arr, arr + sizeof(arr) / sizeof(arr[0]));
	int i = 1;
	while (getline(file, line))
	{
		std::stringstream ss;
		if (line.find(";") == std::string::npos or line.find(";") != line.size() - 1)//TODO : [M] check is not block
			throw std::runtime_error("Line : " + std::to_string(i) + " : syntax error : missing `;`");
		ss << line;
		ss >> key;
		ss >> value;
		value = line.substr(line.find(value) - 1);// TODO :[A] check if value is empty of equal to ";"
		if (lt.find(key) == lt.end())
			throw std::runtime_error("Line : " + std::to_string(i) + " : syntax error : unknown `" + key + "`");
		else
			check_value(key, value, i);
		token += key + value + " ";
		i++;
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