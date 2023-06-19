/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 15:44:45 by moseddik          #+#    #+#             */
/*   Updated: 2023/06/19 10:51:37 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "WebServ.hpp"

std::vector<std::string> split(std::string &str, char delim, bool keepDelim);
bool	isDirectory( const std::string & path );
bool	isFile( const std::string & path );
bool	beginsWith( const std::string & str, const std::string & prefix );
void	print_servers(Server &servers, int ind);
bool	checkDirFile(std::string &path, std::string const &index);
std::string const generateDirectory(std::string const &path);
bool findIf( std::map<std::string, std::string> &headers, std::vector<std::string> const v );
void removeDirectory(const char* path);
std::string getStatusMessage( uint16_t status );

template <typename T>
bool	findVector(T key, std::vector<T> vec)
{
	std::vector<std::string> splitVec = split(vec[0], ' ', false);

	for (size_t i = 0; i < splitVec.size(); i++)
	{
		if (splitVec[i] == key)
			return true;
	}
	return false;
}