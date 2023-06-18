/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 15:44:45 by moseddik          #+#    #+#             */
/*   Updated: 2023/06/18 17:14:30 by moseddik         ###   ########.fr       */
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
std::string getStatusMessage( uint16_t status );

template <typename T>
bool	findVector(T key, std::vector<T> vec)
{
	std::vector<std::string> splitVec = split(vec[0], ' ', false);

	for (size_t i = 0; i < splitVec.size(); i++)
	{
		std::cout << "splitVec :" << splitVec[i] << std::endl;
		std::cout << "key :" << key << std::endl;
		if (splitVec[i] == key)
			return true;
	}
	return false;
}