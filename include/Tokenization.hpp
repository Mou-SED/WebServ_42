/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenization.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 18:37:58 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/06/09 11:11:07 by aaggoujj         ###   ########.fr       */
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
	std::string getServerName( void ) const;
	std::string getHostListen( void ) const;
	std::string getRoot( void ) const;
	std::string getContext(std::string &url) const;
};

void check_value(std::string const & key, std::string const & value);

std::string get_value(std::string const &str, size_t pos);
std::vector<Server>	Tokenization(std::ifstream & file);
