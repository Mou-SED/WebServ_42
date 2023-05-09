/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenization.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 18:37:58 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/05/09 14:20:27 by moseddik         ###   ########.fr       */
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
};

std::string  get_value(std::string const &str, size_t pos);
std::vector<Server>	Tokenization(std::ifstream & file);
