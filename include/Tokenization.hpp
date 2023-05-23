/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenization.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 18:37:58 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/05/23 12:46:02 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "WebServ.hpp"

enum Tokens
{
	TOKEN_NONE,
	TOKEN_LEFT_BRACKET,
	TOKEN_RIGHT_BRACKET,
	TOKEN_SERVER,
	TOKEN_LOCATION,
	TOKEN_SEMICOLON,
	TOKEN_NAME,
	TOKEN_VALUE,
	TOKEN_INITIALIZED,
};

struct Server
{
	std::map<std::string, std::vector<std::string > > directives;
	std::vector< std::pair<std::string, std::map<std::string, std::vector<std::string > > > > context;

	size_t getPort( void ) const;
	std::string getHost( void ) const;
	std::string getHostListen( void ) const;
};

void check_value(std::string const & key, std::string const & value);

std::string get_value(std::string const &str, size_t pos);
std::vector<Server>	Tokenization(std::ifstream & file);
