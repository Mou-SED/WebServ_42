/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/07 22:21:18 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/05/18 17:35:37 by moseddik         ###   ########.fr       */
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/event.h>
#include <fcntl.h>
#include <poll.h>

#define MAX_EVENTS 1024
#define MAX_BUFFSIZE 1048576
enum ERRORS {
		CONTINUE              = 100,
		SWITCHING_PROTOCOLS   = 101,
		PROCESSING            = 102,
		OK                    = 200,
		CREATED               = 201,
		ACCEPTED              = 202,
		NO_CONTENT            = 204,
		PARTIAL_CONTENT       = 206,
		SPECIAL_RESPONSE      = 300,
		MOVED_PERMANENTLY     = 301,
		MOVED_TEMPORARILY     = 302,
		SEE_OTHER             = 303,
		NOT_MODIFIED          = 304,
		TEMPORARY_REDIRECT    = 307,
		PERMANENT_REDIRECT    = 308,
		BAD_REQUEST           = 400,
		UNAUTHORIZED          = 401,
		FORBIDDEN             = 403,
		NOT_FOUND             = 404,
		METHOD_NOT_ALLOWED    = 405,
		REQUEST_TIME_OUT      = 408,
		CONFLICT              = 409,
		LENGTH_REQUIRED       = 411,
		PRECONDITION_FAILED   = 412,
		REQUEST_ENTITY_TOO_LARGE  = 413,
		REQUEST_URI_TOO_LARGE     = 414,
		UNSUPPORTED_MEDIA_TYPE    = 415,
		RANGE_NOT_SATISFIABLE     = 416,
		TOO_MANY_REQUESTS         = 429,
		INTERNAL_SERVER_ERROR     = 500,
		NOT_IMPLEMENTED           = 501,
		BAD_GATEWAY               = 502,
		SERVICE_UNAVAILABLE       = 503,
		GATEWAY_TIME_OUT          = 504,
		VERSION_NOT_SUPPORTED     = 505
};

void	check_syntax(std::ifstream &file);
bool	webserv(const char *av);
bool	parcing(std::ifstream &file, std::string s);
std::string	trim( std::string const & str );
std::string trim( std::string const & str, std::string const & whitespace);

/////////////////////////// check_value.cpp ///////////////////////////

void	listen_check(std::string const & value);
void	server_name_check(std::string const & value);
void	error_page_check(std::string const & value);
void	client_max_body_size_check(std::string const & value);
void	autoindex_check(std::string const & value);
void	root_check(std::string const & value);
void	index_check(std::string const & value);
void	location_check(std::string const & value);

