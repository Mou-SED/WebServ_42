/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 15:12:49 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/07/20 09:18:45 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/WebServ.hpp"
#include "../include/Cgi.hpp"

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

std::string getMatching(std::string const & s1, std::string const & s2)
{
	for (long i = s2.size() - 1; i >= 0; i--)
	{
		if (s1.find(s2.substr(0, i)) != std::string::npos and s2.substr(0, i).size() > 1)
			return s1.substr(0, s1.find(s2.substr(0, i))) + s2.substr(0, i) + s2.substr(i);
	}
	return s1+s2;
}

Cgi::Cgi(std::string const &path, std::string const &method, Request &req, std::pair<std::string, Directives > * location )
{
	std::vector<std::string> tmp = split(location->second["cgi_pass"][0], ' ', false);

	try{
		setApp(tmp[1]);
		setPath(getMatching(location->second["root"][0] , path.substr(0, path.find_first_of("?"))));
		this->_method = method;
		setFds(method);
		this->_headers = req.getHeaders();
		this->_query = getQuery(path);
		this->_fileBody = req.file;
		this->_sizeBody =req._bodySize;
		this->state = stateStar;
		this->bodySize = 0;
		this->_status = req.status;
	}
	catch(const std::exception& e)
	{
		this->_status = INTERNAL_SERVER_ERROR;
	}
	return ;
}

Cgi & Cgi::operator=(Cgi const & rhs)
{
	if (this != &rhs)
	{
		this->_contentType = rhs._contentType;
		this->_setCookie = rhs._setCookie;
		this->_response = rhs._response;
		this->_location = rhs._location;
		this->_cookies = rhs._cookies;
		this->_headers = rhs._headers;
		this->_method = rhs._method;
		this->_query = rhs._query;
		this->_body = rhs._body;
		this->_path = rhs._path;
		this->_env = rhs._env;
		this->_app = rhs._app;
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
	else if (this->pid == 0)
		this->childProcess();
	else
		this->parentProcess();
	return 0;
}

void Cgi::childProcess(void)
{
	dup2(fds[CGI_WRITE], 1);
	close(fds[SERVER_READ]);
	close(fds[CGI_WRITE]);
	if (this->_method == "POST")
	{
		dup2(fds[CGI_READ], 0);
		close(fds[SERVER_WRITE]);
		close(fds[CGI_READ]);
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
	if (chdir(this->_path.substr(0, this->_path.find_last_of('/')).c_str()) != 0)
		exit(1);
	execve(args[0], args, env);
	for (size_t i = 0; i < this->_env.size(); i++)
		delete[] env[i];
	delete[] env;
	std::cerr << "execve failed: " << args[0] << " " << args[1] << std::endl;
	delete[] args[0];
	delete[] args[1];
	delete[] args;
	exit(1);
}

off_t	Cgi::sizeFile( void )
{
	struct stat st;

	if (stat(this->_fileBody.path.c_str(), &st) == -1)
		throw std::runtime_error("Error: stat failed");
	return st.st_size;
}

void	Cgi::sendingBody( void )
{
	time_t	t = time(NULL);
	off_t	size = 0;
	char	buffer[MYBUFSIZ] = {0};
	int		ret, bytes;

	close(fds[CGI_READ]);
	fcntl(fds[SERVER_WRITE], F_SETFL, O_NONBLOCK);
	while (size < this->_sizeBody)
	{
		if (time(NULL) - t > 50)
			throw std::runtime_error("Error : CGI timeout");
		ret = read(this->_fileBody.WR_fd, buffer, MYBUFSIZ);
		bytes = write(fds[SERVER_WRITE], buffer, ret);
		if (bytes > 0)
			size += bytes;
	}
	if (this->_fileBody.WR_fd > 3)
		close(this->_fileBody.WR_fd);
	close(fds[SERVER_WRITE]);
}

void	Cgi::addToHeaders(std::string &str, bool &body)
{
	size_t pos = str.find(CRLF2);
	size_t i = 0;

	while (i < pos)
	{
		i = str.find(CRLF);
		if (i == std::string::npos or i == 0)
			break;
		std::string tmp = str.substr(0, i);
		i += 2;
		str = str.substr(i);
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
	if (str.find(CRLF2) != std::string::npos)
		body = true;
	this->_body = str.substr(str.find(CRLF2) + 3);
}

void	Cgi::waitingChild( void )
{
	int		ret;
	int		now = time(NULL);
	int		_time = time(NULL);

	while (now - _time < 5)
	{
		ret = waitpid(this->pid, &this->exStatus, WNOHANG | WUNTRACED);
		if (ret == -1)
		{
			  if (errno != EAGAIN && errno != EWOULDBLOCK)
			  	return ;
		}
		else {
		{
			if ((WIFEXITED(this->exStatus) == true and WEXITSTATUS(this->exStatus) != EXIT_SUCCESS) or this->exStatus == 1)
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
	close(fds[CGI_WRITE]);
	std::string	str;
	time_t		t = time(NULL);
	char		buffer[MYBUFSIZ] = {0};
	bool		body = false;
	int			ret;

	try
	{
		if (this->_method == "POST")
			sendingBody();
		while ( 1337 )
		{
			if (this->_body.find(CRLF2) != std::string::npos)
					break;
			if (time(NULL) - t > 10)
			{
				this->_status = GATEWAY_TIME_OUT;
				break;
			}
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			ret = ::read(fds[SERVER_READ], buffer, MYBUFSIZ);
			if (ret == 0 or ret == -1)
				break;
			str.append(buffer, ret);
			if (str.find(CRLF2) != std::string::npos and body == false)
				addToHeaders(str, body);
			else
				this->_body.append(buffer, ret);
			if (this->_body.find(CRLF2) != std::string::npos or this->_body.find("\n\n") != std::string::npos)
				break;
		}
		close(fds[SERVER_READ]);
		waitingChild();
	}
	catch(const std::exception& e)
	{
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
	this->_env.push_back("SCRIPT_FILENAME=" + this->_path.substr(this->_path.find_last_of("/") + 1) );
	if (this->_headers.find("cookie") != this->_headers.end() )
		this->_env.push_back("HTTP_COOKIE=" + this->_headers["cookie"] );
	return;
}

void Cgi::setApp(std::string const &app)
{
	this->_app = app;
	checkApp(app.c_str());
}

void Cgi::setPath(std::string const & path)
{
 	if (access(path.c_str(), F_OK) == -1)
		throw std::runtime_error("Error: File not found.");
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

void	Cgi::setFds(std::string const & methode)
{
	if (methode == "GET")
		this->fds = std::vector<int>(2);
	else
		this->fds = std::vector<int>(4);
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
	std::vector<std::string> paths;
	std::string	path;

	if (getenv("PATH") == NULL)
		return false;
	path = getenv("PATH");
	paths = split(path, ':', false);
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
	else if (!(sb.st_mode & S_IXUSR))
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
		headers += it->first + ": " + it->second + CRLF;
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
	return this->_setCookie;
}

std::string Cgi::getLocation(void) const
{
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
	this->_contentType.clear();
	this->_setCookie.clear();
	this->_response.clear();
	this->_location.clear();
	this->_cookies.clear();
	this->_headers.clear();
	this->_method.clear();
	this->_query.clear();
	this->_body.clear();
	this->_path.clear();
	this->_env.clear();
	this->_app.clear();
}
