/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 15:44:24 by moseddik          #+#    #+#             */
/*   Updated: 2023/06/18 17:14:18 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Utils.hpp"
#include <sys/stat.h>

std::vector<std::string> split(std::string const &str, char delim, bool keepDelim)
{
	std::vector<std::string> result;
	size_t start = 0;

	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == delim)
		{
			if ( keepDelim )
				result.push_back(str.substr(start, i - start + 1));
			else
				result.push_back(str.substr(start, i - start));
			start = i + 1;
		}
	}
	if (start < str.size())
		result.push_back(str.substr(start));

	return result;
}

std::vector<std::string> split(std::string &str, char delim, bool keepDelim)
{
	std::vector<std::string> result;
	size_t start = 0;

	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == delim)
		{
			if ( keepDelim )
				result.push_back(str.substr(start, i - start + 1));
			else
				result.push_back(str.substr(start, i - start));
			start = i + 1;
		}
	}
	if (start < str.size())
		result.push_back(str.substr(start));

	return result;
}

bool	isDirectory( const std::string & path )
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
			return true;
	}
	return false;
}

bool	isFile( const std::string & path )
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFREG)
			return true;
	}
	return false;
}

bool	beginsWith( const std::string & str, const std::string & prefix )
{
	return str.substr(0, prefix.size()) == prefix;
}

void	print_servers(Server &servers, int ind)
{
	std::cout << "Sever block" << std::endl;
	for (int i = 0; i < ind; i++)
		std::cout << "---";
	std::cout << "Directives:" << std::endl;
	std::cout << servers.directives.size() << std::endl;
	for (const auto& [key, values] : servers.directives)
	{
		std::cout << "- " << key << ": ";
		for (const auto& value : values)
		{
			std::cout << value << " ";
		}
		std::cout << std::endl;
	}

	std::cout << "Contexts:" << std::endl;
	for (const auto& [name, context] : servers.context)
	{
		std::cout << "- " << name << ":" << std::endl;
		for (const auto& [key, values] : context)
		{
			std::cout << "  - " << key << ": ";
			for (const auto& value : values)
			{
				std::cout << value << " ";
			}
			std::cout << std::endl;
		}
	}

}

bool	checkDirFile(std::string &path, std::string const &index)
{
	std::vector<std::string> indexes = split(const_cast<std::string &>(index), ' ', false);
	for (size_t i = 0; i < indexes.size(); i++)
	{
		DIR *dir;
		struct dirent *ent;
		bool ret = false;

		if ((dir = opendir(path.c_str())) != NULL)
		{
			while ((ent = readdir(dir)) != NULL)
			{
				if ( ent->d_name == indexes[i] )
				{
					path += indexes[i];
					ret = true;
					break ;
				}
			}
			closedir(dir);
		}
		if (ret == true)
			return ret;
	}
	return false;
}

std::string const generateDirectory(std::string const &path)
{
    std::stringstream ss;
    ss << "<!DOCTYPE html>\n<html>\n\t<head>\n\t\t<title>Directory Listing</title>\n\t\t<style>\n\t\t\ttable {\n\t\t\t\tborder-collapse: collapse;\n\t\t\t\tmargin: auto;\n\t\t\t\twidth: 100%;\n\t\t\t}\n\t\t\tth, td {\n\t\t\t\tpadding: 8px;\n\t\t\t\ttext-align: left;\n\t\t\t\tborder-bottom: 1px solid #ddd;\n\t\t\t}\n\t\t\tth {\n\t\t\t\tbackground-color: #f2f2f2;\n\t\t\t}\n\t\t</style>\n\t</head>\n\t<body>\n\t\t<h1>Directory Listing</h1>\n\t\t<hr>\n\t\t<table>\n\t\t\t<tr><th>Type</th><th>Name</th><th>Size (bytes)</th><th>Creation Date</th><th>Extension</th></tr>\n";

    DIR* dir = opendir(path.c_str());
    if (dir != NULL) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] == '.') {
                continue;
            }
            struct stat fileStat;
            std::string entryPath = path + "/" + entry->d_name;
            if (stat(entryPath.c_str(), &fileStat) < 0) {
                std::cerr << "Error: Failed to get file stats for " << entryPath << std::endl;
                continue;
            }
            ss << "\t\t\t<tr>\n";
            if (S_ISDIR(fileStat.st_mode)) {
                ss << "\t\t\t\t<td>Directory</td>\n";
                ss << "\t\t\t\t<td><a href=\"" << entry->d_name << "/\">" << entry->d_name << "/</a></td>\n";
                ss << "\t\t\t\t<td></td>\n";
                ss << "\t\t\t\t<td>" << std::put_time(std::gmtime(&fileStat.st_ctime), "%Y-%m-%d %H:%M:%S") << "</td>\n";
                ss << "\t\t\t\t<td></td>\n";
            } else if (S_ISREG(fileStat.st_mode)) {
                ss << "\t\t\t\t<td>File</td>\n";
                ss << "\t\t\t\t<td><a href=\"" << entry->d_name << "\">" << entry->d_name << "</a></td>\n";
                ss << "\t\t\t\t<td>" << fileStat.st_size << "</td>\n";
                ss << "\t\t\t\t<td>" << std::put_time(std::gmtime(&fileStat.st_ctime), "%Y-%m-%d %H:%M:%S") << "</td>\n";
                ss << "\t\t\t\t<td>" << std::string(entry->d_name).substr(std::string(entry->d_name).find_last_of(".") + 1) << "</td>\n";
            }
            ss << "\t\t\t</tr>\n";
        }
        closedir(dir);
    } else {
        std::cerr << "Error: Failed to open directory " << path << std::endl;
    }

    ss << "\t\t</table>\n";
    ss << "\t</body>\n";
    ss << "</html>\n";
    return ss.str();
}

bool findIf( std::map<std::string, std::string> &headers, std::vector<std::string> const v )
{
	for (size_t i = 0; i < v.size(); i++)
	{
		if (headers.find(v[i]) == headers.end())
			return true;
	}

	return false;
}

std::string getStatusMessage( uint16_t status )
{
	switch(status)
	{
		case OK:
			return "OK";
		case CREATED:
			return "Created";
		case VERSION_NOT_SUPPORTED:
			return "HTTP Version Not Supported";
		case NO_CONTENT:
			return "No Content";
		case MOVED_PERMANENTLY:
			return "Moved Permanently";
		case MOVED_TEMPORARILY:
			return "Moved Temporarily";
		case BAD_REQUEST:
			return "Bad Request";
		case FORBIDDEN:
			return "Forbidden";
		case NOT_FOUND:
			return "Not Found";
		case INTERNAL_SERVER_ERROR:
			return "Internal Server Error";
		case NOT_IMPLEMENTED:
			return "Not Implemented";
		case BAD_GATEWAY:
			return "Bad Gateway";
		case CONFLICT:
			return "Conflict";
		case METHOD_NOT_ALLOWED:
			return "Not Allowed";
		default:
			throw std::runtime_error("Unknown status code");
	}
}
