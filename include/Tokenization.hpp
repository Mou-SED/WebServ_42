/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenization.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 18:37:58 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/07/20 10:48:39 by aaggoujj         ###   ########.fr       */
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

typedef std::map<std::string, std::vector<std::string> > Directives;
struct Server
{
	std::map<std::string, std::vector<std::string > > directives;
	std::vector< std::pair<std::string, Directives > > context;

	size_t getPort( void ) const;
	std::string getServerName( void ) const;
	std::string getHost( void ) const;
	std::string getRoot( void ) const;
	std::string getContext(std::string &url) const;
	std::pair<std::string, Directives > *	matchLocation( std::string & uri );
	std::string getAutoindex( void ) const;
	bool isLocationExist( void ) const;
	void setRootDirectory(std::string root);
};

void check_value(std::string const & key, std::string const & value);

std::string get_value(std::string const &str, size_t pos);
std::vector<Server>	Tokenization(std::ifstream & file);
