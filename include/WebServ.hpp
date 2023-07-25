/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/07 16:20:28 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/07/25 05:50:06 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fstream>
#include <vector>
#include <algorithm>
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <poll.h>
#include <time.h>
#include <iomanip>
#include "Error.hpp"
#include "dirent.h"
#include "Utils.hpp"
#include <sys/time.h>
#include <stdlib.h>
#include <cstdlib>

#define MAX_EVENTS 1024
#define MYBUFSIZ 1048576
#define CRLF "\r\n"
#define CRLF2 "\r\n\r\n"


void	check_syntax(std::ifstream &file);
void	webserv(const char *av);
bool	parcing(std::ifstream &file, std::string s);
std::string	trim( std::string const & str );
std::string trim( std::string const & str, std::string const & whitespace);

void	listen_check(std::string const & value);
void	server_name_check(std::string const & value);
void	error_page_check(std::string const & value);
void	client_max_body_size_check(std::string const & value);
void	autoindex_check(std::string const & value);
void	root_check(std::string const & value);
void	index_check(std::string const & value);
void	location_check(std::string const & value);
void	redirectionCheck(std::string const & value);
void	allowed_methods_check(std::string const & value);

std::vector<std::string> split(std::string const &str, char delim, bool keepIt);
std::vector<std::string> split(std::string &str, char delim, bool keepDelim);