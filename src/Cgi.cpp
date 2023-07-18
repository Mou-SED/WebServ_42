/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 15:12:49 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/07/18 12:55:27 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Cgi.hpp"
#include "../include/WebServ.hpp"
#include <string>
#include <unistd.h>
#include <vector>

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

Cgi::Cgi(std::string const &path, std::string const &method, Request &req, std::pair<std::string, Directives > * location )
{
	std::cerr << "rooooot = " << location->second["root"][0] + path.substr(0, path.find_first_of("?")) << std::endl;//TODO checking root if founded
	std::vector<std::string> tmp = split(location->second["cgi_pass"][0], ' ', false);
	setApp(tmp[1]);
	setPath(location->second["root"][0] + path.substr(0, path.find_first_of("?")));
	std::cerr << "Path -> " << getPath() << std::endl;
	std::cerr << "path file :" << req.file.path	<< std::endl;
	this->_method = method;
	this->_headers = req.getHeaders();
	this->_query = getQuery(path);
	this->_fileBody = req.file;
	this->_sizeBody =req._bodySize;
	this->state = stateStar;
	this->bodySize = 0;
	this->_status = req.status;
	return;
}

Cgi & Cgi::operator=(Cgi const & rhs)
{
	if (this != &rhs)
	{
		this->_path = rhs._path;
		this->_method = rhs._method;
		this->_headers = rhs._headers;
		this->_response = rhs._response;
	}
	return *this;
}

void	Cgi::createPipe(void)
{
	if (pipe(&this->fds[SERVER_READ]) == -1)
		this->_status = INTERNAL_SERVER_ERROR;
	if (this->_method == "POST")
	{
		if (pipe(&this->fds[CGI_READ]) == -1)
			this->_status = INTERNAL_SERVER_ERROR;
	}
}

int Cgi::execute(void)
{
	this->createPipe();
	fcntl(fds[CGI_WRITE], F_SETFL, O_NONBLOCK);
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
	dup2(fds[CGI_WRITE], 1);
	close(fds[SERVER_READ]);
	// close(fds[CGI_WRITE]);
	if (this->_method == "POST")
	{
		dup2(fds[CGI_READ], 0);
		close(fds[SERVER_WRITE]);
		// close(fds[CGI_READ]);
	}
	setEnv();
	char **env = new char*[this->_env.size() + 1];
	for (size_t i = 0; i < this->_env.size(); i++)
		env[i] = strdup(this->_env[i].c_str());
	env[this->_env.size()] = NULL;
	char **args = new char*[3];
	args[0] = strdup(this->_app.c_str());
	args[1] = strdup(this->_path.substr(this->_path.find_last_of("/") + 1).c_str());
	args[2] = NULL;
	std::cerr << "execve : "<< args[0] << "$ $" << args[1] << "$" << std::endl;
	std::cerr << "chdir = " << this->_path.substr(0, this->_path.find_last_of('/')) << std::endl;
	if (chdir(this->_path.substr(0, this->_path.find_last_of('/')).c_str()) != 0)
		std::cerr << "Erroor\n";
	std::cerr << "pwd : " << getcwd(NULL, 0) << std::endl;
	execve(args[0], args, env);
	for (size_t i = 0; i < this->_env.size(); i++)
		delete[] env[i];
	delete[] env;
	delete[] args;
	std::cerr << "execve failed" << std::endl;
	exit(1);
}

off_t	Cgi::sizeFile( void )
{
	struct stat st;
	if (stat(this->_fileBody.path.c_str(), &st) == -1)
		throw std::runtime_error("stat failed");
	return st.st_size;
}

void	Cgi::sendingBody( void )
{
	time_t t = time(NULL);

	close(fds[CGI_READ]);
	char buffer[MYBUFSIZ] = {0};
	int ret;
	off_t size = 0;
	std::cerr << this->_sizeBody << std::endl;
	fcntl(fds[SERVER_WRITE], F_SETFL, O_NONBLOCK);
	while (size < this->_sizeBody)
	{
		if (time(NULL) - t > 50)
			throw std::runtime_error("CGI timeout");
		ret = read(this->_fileBody.WR_fd, buffer, MYBUFSIZ);
		std::cerr << "ret: " << ret << std::endl;
		std::cerr << "buffer: " << buffer << std::endl;
		int bytes = write(fds[SERVER_WRITE], buffer, ret);
		std::cerr << "bytes: " << bytes << std::endl;
		if (bytes > 0)
			size += bytes;
	}
	if (this->_fileBody.WR_fd > 3)
		close(this->_fileBody.WR_fd);
	close(fds[SERVER_WRITE]);
	std::cerr << "cgi sending body done" << std::endl;
}

void	Cgi::addToHeaders(std::string &str, bool &body)
{
	size_t pos = str.find("\r\n\r\n");
	size_t i = 0;
	while (i < pos)
	{
		i = str.find("\r\n");
		std::cerr << "i = " << i << std::endl;
		if (i == std::string::npos or i == 0)
			break;
		std::string tmp = str.substr(0, i);
		i += 2;
		str = str.substr(i);
		std::cout << ">> " << tmp << " str = " << str << pos << " " << i << " " << std::endl;
		std::vector<std::string> v(2);
		v[0] = tmp.substr(0, tmp.find(":"));
		v[1] = tmp.substr(tmp.find(":") + 2);
		if (v[0] == "Content-Length")
			this->_sizeBody = std::stoi(v[1]), this->_headers[v[0]] = v[1];
		else if (v[0] == "Status")
			this->_status = std::stoi(v[1]), this->_headers[v[0]] = v[1];
		else if (v[0] == "Location")
			this->_location = v[1], this->_headers[v[0]] = v[1];
		else if (v[0] == "Content-Type")
			this->_contentType = v[1], this->_headers[v[0]] = v[1];
		else if (v[0] == "Set-Cookie")
			this->_setCookie = v[1], this->_headers[v[0]] = v[1];
	}
	if (str.find("\r\n\r\n") != std::string::npos)
		body = true;
	this->_body = str.substr(str.find("\r\n\r\n") + 3);
	// str.clear();
}

void	Cgi::waitingChild( void )
{
	int t = time(NULL);

	int now = time(NULL);
	while (now - t < 5)
	{
		int ret = waitpid(this->pid, &this->exStatus, WNOHANG | WUNTRACED);
		if (ret == -1)
		{
			  if (errno != EAGAIN && errno != EWOULDBLOCK)
			  	return ;
			std::cerr << "waiting child" << std::endl;
		}
		else {
		{
			if ((WIFEXITED(this->exStatus) == true and WEXITSTATUS(this->exStatus) != EXIT_SUCCESS))
				this->_status = BAD_GATEWAY;
			return ;
		}
		}
		
		now = time(NULL);
	}
	this->_status = GATEWAY_TIME_OUT;
}

void	Cgi::parentProcess( void )
{
	std::cerr << "parent process" << std::endl;
	close(fds[CGI_WRITE]);
	char buffer[MYBUFSIZ] = {0};
	int ret;
	bool body = false;
	time_t t = time(NULL);
	try
	{
		if (this->_method == "POST")
		{
			std::cerr << "sending body" << std::endl;
			sendingBody();
		}
		std::string str;
		while ( 1337 )
		{
			std::cerr << this->_body << std::endl;
			if (this->_body.find("\r\n\r\n") != std::string::npos)
					break;
			std::cerr << "!!!reading cgi " << time(NULL) - t << std::endl;
			if (time(NULL) - t > 10)
			{
				this->_status = GATEWAY_TIME_OUT;
				break;
			}
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			ret = ::read(fds[SERVER_READ], buffer, MYBUFSIZ);
			std::cerr << "heeey\n";
			std::cerr << "--ret: " << ret << std::endl;
			if (ret == 0 or ret == -1)
				break;
			str.append(buffer, ret);
			if (str.find("\r\n\r\n") != std::string::npos and body == false)
			{
				addToHeaders(str, body);
			}
			else
			{
				std::cerr << "is body" << std::endl;
				this->_body.append(buffer, ret);
			}
			if (this->_body.find("\r\n\r\n") != std::string::npos or this->_body.find("\n\n") != std::string::npos)
				break;
		}
		
		close(fds[SERVER_READ]);
		std::cerr << "cgi read done" << std::endl;
		waitingChild();
		std::cerr << "waiting child done" << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << "++++++++++++++++\n";
		kill(this->pid, SIGKILL);
		this->_status = GATEWAY_TIME_OUT;
		return ;
	}
}

void Cgi::setEnv(void)
{
	this->_env.push_back("REQUEST_METHOD=" + this->_method );
	this->_env.push_back("CONTENT_LENGTH=" + std::to_string(this->_sizeBody));
	this->_env.push_back("CONTENT_TYPE=" + this->_headers["Content-Type"] );
	this->_env.push_back("QUERY_STRING=" + this->_query );
	this->_env.push_back("REDIRECT_STATUS=200" );
	this->_env.push_back("SERVER_NAME=" + this->_headers["Host"] );
	this->_env.push_back("SERVER_PORT=" + this->_headers["Host"] );
	this->_env.push_back("SERVER_PROTOCOL=HTTP/1.1" );
	this->_env.push_back("SERVER_SOFTWARE=WebServ/0.0.1" );
	this->_env.push_back("GATEWAY_INTERFACE=CGI/1.1" );
	this->_env.push_back("REMOTE_ADDR=" + this->_headers["Host"] );
	std::cerr << "sript  = " << this->_path.substr(this->_path.find_last_of("/") + 1) << std::endl;
	this->_env.push_back("SCRIPT_FILENAME=" + this->_path.substr(this->_path.find_last_of("/") + 1));
	if (this->_headers.find("cookie") != this->_headers.end())
		this->_env.push_back("HTTP_COOKIE=" + this->_headers["cookie"] );
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

void Cgi::setQuery(std::string &query)
{
	this->_query = query;
}

void Cgi::addHeader(std::string &key, std::string &value)
{
	this->_headers.insert(make_pair(key, value));
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

std::string Cgi::getHeaderString(void) const
{
	std::string headers;
	for (auto it : this->_headers)
	{
		headers += it.first + ": " + it.second + "\r\n";
	}
	return headers;
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

std::string Cgi::getHeaders(void) const
{
	std::string headers;
	for (std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end();++it)
	{
		headers += it->first + ": " + it->second + "\r\n";
	}
	return headers;
}

std::string Cgi::getBody(void) const
{
	return this->_body;
}

std::string Cgi::getResponse(void) const
{
	return this->_response;
}

std::string Cgi::getQuery(std::string const &path) const
{
	
	return path.substr(path.find('?') + 1);
}

std::string Cgi::getCookie(void) const
{
	std::cerr << "Cookie = " << this->_setCookie << std::endl;
	return this->_setCookie;
}

std::string Cgi::getLocation(void) const
{
	std::cerr << "Location = " << this->_location << std::endl;
	return this->_location;
}

std::string Cgi::getContentType(void) const
{
	if (this->_contentType.empty())
		return "text/html";
	return this->_contentType;
}



uint16_t Cgi::getStatus(void) const
{
	return this->_status;
}

off_t Cgi::getSizeBody(void) const
{
	return this->_body.size();
}

void Cgi::clear(void)
{
	this->_path.clear();
	this->_method.clear();
	this->_headers.clear();
	this->_response.clear();
}
