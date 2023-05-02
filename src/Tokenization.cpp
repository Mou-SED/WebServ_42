/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenization.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 18:32:41 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/05/02 21:35:18 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

extern std::string type_value[];

Tokens get_tokens(std::string const & value)
{
	std::set<std::string> tokens;
	for (int i = 0; i < 11; i++)
		if (type_value[i] != "server" and type_value[i] != "location")
			tokens.insert(type_value[i]);
	if (value == "server")
		return (TOKEN_SERVER);
	else if (value == "location")
		return (TOKEN_LOCATION);
	else if (value == "{")
		return (TOKEN_LEFT_BRACKET);
	else if (value == "}")
		return (TOKEN_RIGHT_BRACKET);
	else if (value == ";")
		return (TOKEN_SEMICOLON);
	else if (tokens.find(value) != tokens.end()) // TODO : check if it's a good idea to use name and value as key
		return (TOKEN_NAME);
	else if (value == "value")
		return (TOKEN_VALUE);
	else
		return (TOKEN_NONE);
}

Server get_directive(std::string const & key, std::string const & value)
{
	Server server;
	server.directives[key].push_back(value); // TODO check all the directives don't have the same key like "root, ..., etc"
	return (server);
}

void	parc_location(std::ifstream & file, std::string & value, Server & servers)
{
	std::string line;
	Tokens token;

	servers.context.push_back(std::make_pair(value, std::map<std::string, std::vector<std::string > >()));
	while (getline(file, line))
	{
		std::string key;
		std::string value;
		std::stringstream ss(line);
		ss >> key;
		value = get_value(line, key.size());
		token = get_tokens(key);
		if (token == TOKEN_RIGHT_BRACKET)
			break;
		else if (token == TOKEN_NAME)
			servers.context[servers.context.size() - 1].second[key].push_back(value);
	}
}

Server	parc_server(std::ifstream & file, std::string & line)
{
	Tokens token;
	Server server;

	while (getline(file, line))
	{
		std::string key;
		std::string value;
		std::stringstream ss(line);
		ss >> key;
		value = get_value(line, key.size());
		token = get_tokens(key);
		if (token == TOKEN_RIGHT_BRACKET)
			break;
		else if (token == TOKEN_LOCATION)
			parc_location(file, value, server);
		else if (token == TOKEN_NAME)
		{
			server.directives[key].push_back(value);
		}
	}
	return (server);
}



std::vector<Server>	Tokenization(std::ifstream & file)
{
	std::string line;
	Tokens token;
	std::vector<Server> servers;

	while (getline(file, line))
	{
		std::string key;
		std::stringstream ss(line);
		ss >> key;
		token = get_tokens(key);
		if (token == TOKEN_SERVER) //TODO : part_server
			servers.push_back(parc_server(file, line));
		else if (token == TOKEN_LOCATION) // TODO : part_location
		{
			if (servers.size() == 0)
				throw std::runtime_error("Syntax error");
			else
				parc_location(file, line, servers[servers.size() - 1]);
		}
	}
	return (servers);
}