/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenization.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 18:32:41 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/05/07 16:35:13 by aaggoujj         ###   ########.fr       */
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
	if (server.directives.find(key) != server.directives.end() and (key == "root" or key == "client_max_body_size" or key == "autoindex"))
		throw std::runtime_error("Syntax error : directive already exist");
	server.directives[key].push_back(value); // TODO check all the directives don't have the same key like "root, ..., etc"
	return (server);
}

void	parc_location(std::ifstream & file, std::string & value, Server & servers)
{
	std::string line;
	Tokens token;

	value = value.substr(0, value.find_first_of(" {"));
	servers.context.push_back(std::make_pair(value, std::map<std::string, std::vector<std::string > >()));
	while (getline(file, line))
	{
		std::string key;
		std::string value;
		std::stringstream ss(line);
		ss >> key;
		value = get_value(line, key.size());
		value = value.substr(0, value.find_first_of(";"));
		token = get_tokens(key);
		if (token == TOKEN_RIGHT_BRACKET)
			break;
		else if (token == TOKEN_NAME)
		{
			if (servers.context[servers.context.size() - 1].second.find(key) != servers.context[servers.context.size() - 1].second.end() and
				(key == "root" or key == "client_max_body_size" or key == "autoindex"))
				throw std::runtime_error("Syntax error : " + key + " directive already exist");
			servers.context[servers.context.size() - 1].second[key].push_back(value);
		}
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
		value = value.substr(0, value.find_first_of(";"));
		token = get_tokens(key);
		if (token == TOKEN_RIGHT_BRACKET)
			break;
		else if (token == TOKEN_LOCATION)
			parc_location(file, value, server);
		else if (token == TOKEN_NAME)
		{
			if (server.directives.find(key) != server.directives.end() and
				(key == "root" or key == "client_max_body_size" or key == "autoindex"))
				throw std::runtime_error("Syntax error : " + key + " directive already exist");
			server.directives[key].push_back(value);
		}
	}
	return (server);
}

void is_in_vector(std::vector<std::string> & v, std::string const & value)
{
	for (size_t i = 0; i < v.size(); i++)
		if (v[i].find(value) == std::string::npos)
			v[i] = "0.0.0.0:" + v[i];
}

void	setDefaultDirectives( std::vector<Server> & servers )
{
	for (size_t i = 0; i < servers.size(); i++)
    {
		is_in_vector(servers[i].directives["listen"], ":");
		if ( servers[i].directives.find("root") == servers[i].directives.end() )
			servers[i].directives["root"].push_back("/var/www/html");
		if ( servers[i].directives.find("listen") == servers[i].directives.end() )
			servers[i].directives["listen"].push_back("0.0.0.0:8080");
		if ( servers[i].directives.find("server_name") == servers[i].directives.end() )
			servers[i].directives["server_name"].push_back(servers[i].directives["listen"][0].substr(0, servers[i].directives["listen"][0].find_first_of(":")));
		if ( servers[i].directives.find("error_page") == servers[i].directives.end() )
			servers[i].directives["error_page"].push_back("404 /404.html");
    }
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
	setDefaultDirectives(servers);
	return (servers);
}