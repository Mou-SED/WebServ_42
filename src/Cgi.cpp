/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 15:12:49 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/06/18 17:37:37 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

std::map<std::string, std::string> uncode = {
	{"%20", " "},{"%21", "!"},{"%22", "\""},{"%23", "#"},{"%24", "$"},{"%25", "%"},{"%26", "&"},{"%27", "'"},{"%28", "("},{"%29", ")"},{"%2A", "*"},{"%2B", "+"},{"%2C", ","},{"%2D", "-"},{"%2E", "."},{"%2F", "/"},{"%3A", ":"},{"%3B", ";"},{"%3C", "<"},{"%3D", "="},{"%3E", ">"},{"%3F", "?"},{"%40", "@"},{"%5B", "["},{"%5C", "\\"},{"%5D", "]"},{"%5E", "^"},{"%5F", "_"},{"%60", "`"},{"%7B", "{"},{"%7C", "|"},{"%7D", "}"},{"%7E", "~"}
};

Cgi::Cgi(void)
{
	return;
}

Cgi::~Cgi(void)
{
	return;
}

Cgi::Cgi(Cgi const & src)
{
	*this = src;
	return;
}

Cgi & Cgi::operator=(Cgi const & rhs)
{
	if (this != &rhs)
	{
		this->_path = rhs._path;
		this->_method = rhs._method;
		this->_headers = rhs._headers;
		this->_body = rhs._body;
		this->_response = rhs._response;
	}
	return *this;
}

int Cgi::execute(void)
{
	if (pipe(fds) == -1)
		return -1;
	fcntl(fds[1], F_SETFL, O_NONBLOCK);
	this->pid = fork();
	if (this->pid == -1)
		return -1;
	if (this->pid == 0)
		this->childProcess();
	else
		this->parentProcess();
	return 0;
}

void Cgi::childProcess(void)
{
	std::cerr << "child process" << std::endl;
	close(fds[0]);
	dup2(fds[1], 1);
	close(fds[1]);
	char **args = new char*[3];
	std::cerr << "path: " << this->_path << std::endl;
	std::cerr << "app: " << this->_app << std::endl;
	args[0] = strdup(this->_app.c_str());
	args[1] = strdup(this->_path.c_str());
	args[2] = NULL;
	execve(args[0], args, NULL);
	delete[] args;
	std::cerr << "execve failed" << std::endl;
	exit(0);
}

void	Cgi::parentProcess(void)
{
	std::cerr << "parent process" << std::endl;
	close(fds[1]);
	char buffer[4096];
	int ret;
	std::stringstream ss;
	while ((ret = read(fds[0], buffer, 4096)) > 0)
		ss.write(buffer, ret);
	this->_response = ss.str();
	close(fds[0]);
	waitpid(this->pid, &this->status, 0);
}

void Cgi::addQuery(std::string &key, std::string &value)
{
	this->_query.insert(make_pair(key, value));
}
void Cgi::setApp(std::string const &app)
{
	this->_app = app;
	checkApp(app.c_str());
}

void Cgi::setPath(std::string const & path)
{
	if (access(path.c_str(), F_OK) == -1)
		throw std::runtime_error("file not found");
	this->_path = path;
}

void Cgi::setMethod(std::string& method)
{
	this->_method = method;
}

void Cgi::setHeaders(std::map<std::string, std::string>& headers)
{
	this->_headers = headers;
}

void Cgi::setBody(std::string &body)
{
	this->_body = body;
}

void Cgi::addHeader(std::string &key, std::string &value)
{
	this->_headers.insert(make_pair(key, value));
}

void Cgi::decode(std::string &str)
{
	int pos = str.find("%");
	std::string key = str.substr(str.find("%"), pos + 3);
	str = str.substr(0, pos -1) + uncode[key] + str.substr(pos + 3);
	
}

void	Cgi::creatQuery(std::string &query)
{
	std::vector<std::string> tmp = split(query, '&', false);
	for (size_t i = 0; i < tmp.size(); i++)
	{
		
		std::vector<std::string> tmp2 = split(tmp[i], '=', false);
		if (tmp2[1].find("%") != std::string::npos)
		{
			decode(tmp2[1]);
			
		}
		if (tmp2.size() == 2)
			this->_query.insert(make_pair(tmp2[0], tmp2[1]));
	}
}

bool Cgi::checkApp(const char *env_var)
{
	if (getenv("PATH") == NULL)
		return false;
	std::string path = getenv("PATH");
	std::vector<std::string> paths = split(path, ':', false);
	for (size_t i = 0; i < paths.size();i++)
	{
		std::string tmp = paths[i] + "/" + env_var;
		if (checkPath(tmp))
		{
			this->_app = paths[i] + "/" + env_var;
			return true;
		}
	}
	return false;
}

bool Cgi::checkPath(std::string &path)
{
	struct stat sb;

	if (stat(path.c_str(), &sb) == -1)
		return false;
	if (!(sb.st_mode & S_IXUSR))
		return false;
	return true;
}

std::string Cgi::getPath(void) const
{
	return this->_path;
}

std::string Cgi::getMethod(void) const
{
	return this->_method;
}

std::map<std::string, std::string> Cgi::getHeaders(void) const
{
	return this->_headers;
}

std::string Cgi::getBody(void) const
{
	return this->_body;
}

std::string Cgi::getResponse(void) const
{
	return this->_response;
}

std::string Cgi::getValueQuery(std::string const &key) const
{
	std::map<std::string, std::string>::const_iterator it = this->_query.find(key);
	if (it == this->_query.end())
		return "";
	return it->second;
}

void Cgi::generateResponse(void)
{
	std::string cmd = "php-cgi -f " + this->_path;
	FILE *fp = popen(cmd.c_str(), "r");
	char buf[1024];
	while (fgets(buf, 1024, fp) != NULL)
		this->_response += buf;
	pclose(fp);
}

void Cgi::clear(void)
{
	this->_path.clear();
	this->_method.clear();
	this->_headers.clear();
	this->_body.clear();
	this->_response.clear();
}
