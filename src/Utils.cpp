/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 15:44:24 by moseddik          #+#    #+#             */
/*   Updated: 2023/06/17 20:13:35 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Utils.hpp"
#include <sys/stat.h>

std::vector<std::string> split(std::string const &str, char delim, bool keepDelim)
{
	std::vector<std::string> result;
	size_t start = 0;

	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == delim)
		{
			if ( keepDelim )
				result.push_back(str.substr(start, i - start + 1));
			else
				result.push_back(str.substr(start, i - start));
			start = i + 1;
		}
	}
	if (start < str.size())
		result.push_back(str.substr(start));

	return result;
}

std::vector<std::string> split(std::string &str, char delim, bool keepDelim)
{
	std::vector<std::string> result;
	size_t start = 0;

	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == delim)
		{
			if ( keepDelim )
				result.push_back(str.substr(start, i - start + 1));
			else
				result.push_back(str.substr(start, i - start));
			start = i + 1;
		}
	}
	if (start < str.size())
		result.push_back(str.substr(start));

	return result;
}

bool	isDirectory( const std::string & path )
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
			return true;
	}
	return false;
}

bool	isFile( const std::string & path )
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFREG)
			return true;
	}
	return false;
}

bool	beginsWith( const std::string & str, const std::string & prefix )
{
	return str.substr(0, prefix.size()) == prefix;
}

void	print_servers(Server &servers, int ind)
{
	std::cout << "Sever block" << std::endl;
	for (int i = 0; i < ind; i++)
		std::cout << "---";
	std::cout << "Directives:" << std::endl;
	std::cout << servers.directives.size() << std::endl;
	for (const auto& [key, values] : servers.directives)
	{
		std::cout << "- " << key << ": ";
		for (const auto& value : values)
		{
			std::cout << value << " ";
		}
		std::cout << std::endl;
	}

	std::cout << "Contexts:" << std::endl;
	for (const auto& [name, context] : servers.context)
	{
		std::cout << "- " << name << ":" << std::endl;
		for (const auto& [key, values] : context)
		{
			std::cout << "  - " << key << ": ";
			for (const auto& value : values)
			{
				std::cout << value << " ";
			}
			std::cout << std::endl;
		}
	}

}