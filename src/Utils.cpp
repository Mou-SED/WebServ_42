/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 15:44:24 by moseddik          #+#    #+#             */
/*   Updated: 2023/07/20 07:57:35 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Utils.hpp"
#include <string>
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
                continue;
            }
            ss << "\t\t\t<tr>\n";
            if (S_ISDIR(fileStat.st_mode)) {
                ss << "\t\t\t\t<td>Directory</td>\n";
                ss << "\t\t\t\t<td><a href=\"" << entry->d_name << "/\">" << entry->d_name << "/</a></td>\n";
                ss << "\t\t\t\t<td></td>\n";
                ss << "\t\t\t\t<td>" << std::put_time(gmtime(&fileStat.st_ctime), "%Y-%m-%d %H:%M:%S") << "</td>\n";
                ss << "\t\t\t\t<td></td>\n";
            } else if (S_ISREG(fileStat.st_mode)) {
                ss << "\t\t\t\t<td>File</td>\n";
                ss << "\t\t\t\t<td><a href=\"" << entry->d_name << "\">" << entry->d_name << "</a></td>\n";
                ss << "\t\t\t\t<td>" << fileStat.st_size << "</td>\n";
                ss << "\t\t\t\t<td>" << std::put_time(gmtime(&fileStat.st_ctime), "%Y-%m-%d %H:%M:%S") << "</td>\n";
                ss << "\t\t\t\t<td>" << std::string(entry->d_name).substr(std::string(entry->d_name).find_last_of(".") + 1) << "</td>\n";
            }
            ss << "\t\t\t</tr>\n";
        }
        closedir(dir);
    } else {
        ss << "\t\t\t<tr><td colspan=\"5\">Error: Could not open directory</td></tr>\n";
    }

    ss << "\t\t</table>\n";
    ss << "\t</body>\n";
    ss << "</html>\n";
    return ss.str();
}

bool findIf( std::map<std::string, std::string> &headers, std::string const & v )
{
		if (headers.find(v) == headers.end())
			return true;
	return false;
}

void removeDirectory(const char* path)
{
	DIR* dir = opendir(path);
	dirent* entry;

	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
		{
			char file_path[FILENAME_MAX];
			snprintf(file_path, FILENAME_MAX, "%s/%s", path, entry->d_name);
			if (entry->d_type == DT_DIR) removeDirectory(file_path);
			else remove(file_path);
		}
	}
	closedir(dir);
	rmdir(path);
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
		case REQUEST_ENTITY_TOO_LARGE:
			return "Request Entity Too Large";
		case GATEWAY_TIME_OUT:
			return "Gateway Time Out";
		case REQUEST_URI_TOO_LARGE:
			return "Request URI Too Large";
		case TEMPORARY_REDIRECT:
			return "Temporary Redirect";
		case PERMANENT_REDIRECT:
			return "Permanent Redirect";
		default:
			throw std::runtime_error("Unknown status code" + std::to_string(status));
	}
}
