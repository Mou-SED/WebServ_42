/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/07 22:21:18 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/05/09 14:35:11 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include <string>
#include <set>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Check.hpp"
#include "Tokenization.hpp"

void	check_syntax(std::ifstream &file);
bool	webserv(const char *av);
bool	parcing(std::ifstream &file, std::string s);
std::string	trim( std::string const & str );

/////////////////////////// check_value.cpp ///////////////////////////

void	listen_check(std::string const & value);
void	server_name_check(std::string const & value);
void	error_page_check(std::string const & value);
void	client_max_body_size_check(std::string const & value);
void	autoindex_check(std::string const & value);
void	root_check(std::string const & value);
void	index_check(std::string const & value);
void	location_check(std::string const & value);

