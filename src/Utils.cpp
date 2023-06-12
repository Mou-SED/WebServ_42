/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 15:44:24 by moseddik          #+#    #+#             */
/*   Updated: 2023/06/12 13:30:04 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"
#include <sys/stat.h>

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