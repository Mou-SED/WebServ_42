/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/07 22:21:18 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/04/30 18:15:10 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <set>
#include <stdexcept>

std::string	check_syntax(std::ifstream &file);
bool	webserv(const char *av);
bool	parcing(std::ifstream &file, std::string s);
bool	ipAddress(std::string s);
bool	isPort( std::string s );
std::string	trim( std::string const & str );
bool 		isDomain( std::string const & s );

/////////////////////////// check_value.cpp ///////////////////////////

void	listen_check(std::string const & value, const int i);
void	server_name_check(std::string const & value);
void	error_page_check(std::string const & value);
void	client_max_body_size_check(std::string const & value);
void	autoindex_check(std::string const & value);

