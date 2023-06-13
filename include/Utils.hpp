/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 15:44:45 by moseddik          #+#    #+#             */
/*   Updated: 2023/06/13 15:47:23 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "WebServ.hpp"

std::vector<std::string> split(std::string &str, char delim, bool keepDelim);
bool	isDirectory( const std::string & path );
bool	isFile( const std::string & path );
bool	beginsWith( const std::string & str, const std::string & prefix );
void	print_servers(Server &servers, int ind);