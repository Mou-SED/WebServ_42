/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenization.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 18:32:41 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/06/13 10:21:33 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include "Tokenization.hpp"

extern std::vector<std::string> type_value;

Tokens get_tokens(std::string const &value)
{
	std::set<std::string> tokens(type_value.begin(), type_value.end());
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

Server get_directive(std::string const &key, std::string const &value)
{
	Server server;
	if (server.directives.find(key) != server.directives.end() and (key == "root" or key == "client_max_body_size" or key == "autoindex"))
		throw std::runtime_error("Syntax error : directive already exist");
	server.directives[key].push_back(value); // TODO check all the directives don't have the same key like "root, ..., etc"
	return (server);
}

void parc_location(std::ifstream &file, std::string &value, Server &servers)
{
	std::string line;
	Tokens token;

	value = value.substr(0, value.find_first_of(" {"));
	servers.context.push_back(std::make_pair(value, std::map<std::string, std::vector<std::string>>()));
	while (getline(file, line))
	{
		std::string key;
		std::string value;
		std::stringstream ss(trim(line));

		ss >> key;
		ss >> value;
		token = get_tokens(key);
		value = trim(line.substr(line.find(value)));
		value = value.substr(0, value.find_first_of(";"));
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

Server parc_server(std::ifstream &file, std::string &line)
{
	Tokens token;
	Server server;

	while (getline(file, line))
	{
		std::string key;
		std::string value;
		std::stringstream ss(trim(line));
		ss >> key;
		ss >> value;
		value = trim(line.substr(line.find(value)));
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

void is_in_vector(std::vector<std::string> &v, std::string const &value)
{
	for (size_t i = 0; i < v.size(); i++)
		if (v[i].find(value) == std::string::npos)
			v[i] = "0.0.0.0:" + v[i];
}

void setDefaultDirectives(std::vector<Server> &servers)
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		is_in_vector(servers[i].directives["listen"], ":");
		if (servers[i].directives.find("root") == servers[i].directives.end())
			servers[i].directives["root"].push_back("./www/html");
		if (servers[i].directives.find("listen") == servers[i].directives.end())
			servers[i].directives["listen"].push_back("0.0.0.0:8080");
		if (servers[i].directives.find("server_name") == servers[i].directives.end())
			servers[i].directives["server_name"].push_back(servers[i].directives["listen"][0].substr(0, servers[i].directives["listen"][0].find_first_of(":")));
		if (servers[i].directives.find("error_page") == servers[i].directives.end())
			servers[i].directives["error_page"].push_back("404 /404.html");
	}
}

size_t Server::getPort(void) const
{
	std::string port = directives.at("listen")[0];
	return (std::stoi(port.substr(port.find_first_of(":") + 1)));
}

std::string Server::getServerName(void) const
{
	return directives.find("server_name")->second[0];
}

std::string Server::getHost(void) const
{
	return directives.find("listen")->second[0].substr(0, directives.find("listen")->second[0].find_first_of(":"));
}

std::string Server::getRoot(void) const
{
	return directives.find("root")->second[0];
}

std::string Server::getContext(std::string &url) const
{
	for (size_t i = 0; i < this->context.size(); i++)
	{
		if (url.find(this->context[i].first) != std::string::npos)
			return this->context[i].first;
	}
	return "";
}

std::pair<std::string, Directives > * Server::matchLocation( std::string & uri )
{
	std::pair<std::string, Directives > * bestMatch = nullptr;
	size_t bestMatchLength = 0;

	for (size_t i = 0; i < this->context.size(); i++)
	{
		const std::string & locationUri = this->context[i].first;

		if ( beginsWith( uri, locationUri ) and locationUri.size() > bestMatchLength )
		{
			bestMatch = &this->context[i];
			bestMatchLength = locationUri.size();
		}
	}

	return bestMatch;
}

std::string Server::getAutoindex(void) const
{
	if (this->context.size() > 0)
		return this->context[0].second.find("autoindex")->second[0];
	else
		return this->directives.find("autoindex")->second[0];
}

bool Server::isLocationExist(void) const
{
	return (this->context.size() > 0);
}

std::vector<Server> Tokenization(std::ifstream &file)
{
	std::string line;
	Tokens token;
	std::vector<Server> servers;

	while (getline(file, line))
	{
		std::replace(line.begin(), line.end(), '\t', ' ');
		std::string key;
		std::stringstream ss(line);
		ss >> key;
		token = get_tokens(key);
		if (token == TOKEN_SERVER) // TODO : part_server
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