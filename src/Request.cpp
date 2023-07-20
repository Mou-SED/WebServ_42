/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 17:07:46 by moseddik          #+#    #+#             */
/*   Updated: 2023/07/20 11:33:49 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../include/Request.hpp"

void	Request::checkHeaders(std::map<std::string, std::string> &headers)
{
	if (headers["host"].empty())
	{
		this->status = BAD_REQUEST;
		this->state = ERR;
	}
}

void File::createName( void )
{
	srand(time(NULL));
	for (int i = 0; i < 524288; i++)
	{
		int ch = rand() % 62;

		if (ch < 26)
			name += char(ch + 'a');
		else if (ch < 52)
			name += char(ch + 'A');
		else
			name += char(ch + '0');

		std::string tmp = "/tmp/." + name;
		if (access(tmp.c_str(), R_OK != 0))
			break;
	}
	name = "." + name + ".tmp";
	path = "/tmp/" + name;
}


Request::Request(void)
{
	state = REQUEST_LINE;
	this->isChunked = false;
	this->_chunkedTurn = false;
	this->contentLength = 0;
	this->status = OK;
	this->_server = nullptr;
	_bodySize = 0;
	this->chunkSize = -1;

	return;
}

Request::~Request(void)
{
	if (file.path.size() > 0)
	{
		if (file.file.is_open())
			close(file.WR_fd);
		file.file.close();
		
	}
}

std::string Request::getMethod(void) const
{
	if (this->_method.empty())
		return "";
	return this->_method;
}

std::string Request::getUri(void) const
{
	return this->_uri;
}

std::map<std::string, std::string> Request::getHeaders(void) const
{
	return this->_headers;
}

std::string Request::getBody(void) const
{
	return (this->_body.str());
}

void Request::clear(void)
{
	this->_request.clear();
	this->_method.clear();
	this->_uri.clear();
	this->_headers.clear();
	this->state = REQUEST_LINE;
	this->isChunked = false;
	this->status = 0;
	this->_chunkedBody.str("");
	this->_chunkedBody.clear();
	this->_body.str("");
	this->_body.clear();
}

uint16_t Request::getStatus(void) const
{
	return this->status;
}

Server * Request::getServer(void) const
{
	return this->_server;
}

std::string Request::getContentType()
{
	std::map<std::string, std::string> m;
	m["html"] = "text/html";
	m["htm"] = "text/html";
	m["jpg"] = "image/jpeg";
	m["jpeg"] = "image/jpeg";
	m["png"] = "image/png";
	m["gif"] = "image/gif";
	m["css"] = "text/css";
	m["js"] = "text/javascript";
	m["json"] = "application/json";
	m["xml"] = "application/xml";
	m["pdf"] = "application/pdf";
	m["zip"] = "application/zip";
	m["mp4"] = "video/mp4";
	m["webm"] = "video/webm";
	m["mp3"] = "audio/mp3";
	m["ogg"] = "audio/ogg";
	m["wav"] = "audio/wav";
	m["txt"] = "text/plain";
	m[""] = "application/octet-stream";
	if (m.find(this->_uri.substr(this->_uri.find_last_of(".") + 1)) != m.end())
		return m[this->_uri.substr(this->_uri.find_last_of(".") + 1)];
	else
		return m[""];
}

std::string Request::getContentLength(void)
{
	return std::to_string(this->_body.str().size());
}

size_t Request::getBodySize(void)
{
	return this->_body.str().size();
}

Request & Request::operator=(Request const & rhs)
{
	this->_request << rhs._request.str();
	this->_method = rhs._method;
	this->_uri = rhs._uri;
	this->_headers = rhs._headers;
	this->_body << rhs._body.str();
	this->state = rhs.state;
	this->isChunked = rhs.isChunked;
	this->contentLength = rhs.contentLength;
	this->status = rhs.status;
	this->_server = rhs._server;
	this->_bodySize = rhs._bodySize;
	if (rhs.file.path.size() > 0)
	{
		this->file.name = rhs.file.name;
		this->file.path = rhs.file.path;
		if (this->file.WR_fd > 2)
		close(this->file.WR_fd);
		this->file.WR_fd = open(this->file.path.c_str(), O_RDWR , 0666);
	}

	return *this;
}

bool checkHeaders(std::map<std::string, std::string> headers, std::string key)
{
	if (headers.count(key) != 0 and headers[key].empty())
		return false;
	return true;
}

bool compareStr(std::string const &str1, std::string const &str2)
{
	if (str1.size() > str2.size() or str1.find_first_not_of("0123456789") != std::string::npos)
		return true;
	else if (str1.size() < str2.size())
		return false;
	for (size_t i = 0; i < str1.size(); i++)
	{
		if (str1[i] > str2[i])
			return true;
	}
	return false;
}

bool Request::parsingHeaders(std::vector<std::string> &tokens)
{
	size_t i = 0;

	for ( ; i < tokens.size() && tokens[i].find("\n") != std::string::npos; i++ )
	{
		std::pair<std::string, std::string> header;

		header = make_pair(
			tokens[i].substr(0, tokens[i].find(":")),
			trim(tokens[i].substr(tokens[i].find(":") + 1, tokens[i].find_first_of(CRLF) - tokens[i].find(":") - 1))
		);
		std::transform(header.first.begin(), header.first.end(), header.first.begin(), ::tolower);

		if (tokens[i] != "\n" and tokens[i] != CRLF
				and  ((header.first.empty() or header.second.empty() or not findIf(this->_headers, header.first)) or (tokens[i].find(":") == std::string::npos or tokens[i][tokens[i].find(":") - 1] == ' ' or (header.first == "content-length" and compareStr(header.second, "2147483647")))))
		{
			this->status = BAD_REQUEST;
			this->state = ERR;
			return false;
		}
		if ((tokens[i] == "\n" or tokens[i] == CRLF) and (not this->_headers["content-length"].empty() or not this->_headers["transfer-encoding"].empty()) )
		{
			i++;
			if ( this->_headers["transfer-encoding"] == "chunked" ) this->isChunked = true;
			if (not this->_headers["content-length"].empty()) this->contentLength = std::stoll(this->_headers["content-length"]);
				this->state = BODY;
			this->file.WR_fd = open(this->file.path.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
			if (this->file.WR_fd == -1)
			{
				this->status = INTERNAL_SERVER_ERROR;
				this->state = ERR;
				return false;
			}
			break;
		}
		else if ( tokens[i] == "\n" or tokens[i] == CRLF )
		{
			this->state = DONE;
			checkHeaders(this->_headers);
			break;
		}
		this->_headers[header.first] = trim(header.second, " \r\n");
	}

	this->_request.clear();
	for (size_t j = i; j < tokens.size(); j++)
		this->_request << tokens[j];

	return true;
}

void remove_end(std::string &str, std::string to_remove)
{
	size_t pos = str.find_first_of(to_remove);
	if (pos != std::string::npos)
		str = str.substr(0, pos);
}

bool checkMethod(std::string &line)
{
	std::vector<std::string> list(4);// = {"GET", "POST", "DELETE", "PUT"};

	list[0] = "GET";list[1] = "POST";list[2] = "DELETE";list[3] = "PUT";
	for (size_t i = 0; i < list.size(); i++)
	{
		if (line == list[i])
			return true;
	}

	return false;
}

bool Request::parsingStartLine(std::vector<std::string> &tokens)
{
	remove_end(tokens[0], CRLF);
	std::vector<std::string> requestLine = split(tokens[0], ' ', false);
	if (tokens[0].size() > 2048)
	{
		this->status = REQUEST_URI_TOO_LARGE;
		this->state = ERR;
		return false;
	}
	if (requestLine.size() != 3 or not checkMethod(requestLine[0]))
	{
		if ( requestLine.size() != 3 )
			this->status = BAD_REQUEST;
		else
			this->status = NOT_IMPLEMENTED;
		this->state = ERR;

		return false;
	}
	if (requestLine[2] != "HTTP/1.1")
	{
		this->status = VERSION_NOT_SUPPORTED;
		this->state = ERR;

		return false;
	}

	this->_method = requestLine[0];
	this->_uri = requestLine[1];
	this->state = HEADERS;
	tokens.erase(tokens.begin());

	return true;
}

bool Request::parsing(std::vector<std::string> &tokens)
{
	if ( state == REQUEST_LINE )
	{
		if ( not parsingStartLine(tokens) ) return false;
	}
	if ( state == HEADERS )
	{
		if ( not parsingHeaders(tokens) ) return false;
	}
	if ( state == BODY )
	{
		return mainParsingRequest(this->_request);
	}

	return true;
}

bool Request::readLine(std::stringstream &line)
{
	if ( not this->_request.str().empty() )
	{
		this->_request << line.str();
		line << this->_request.str();
	}

	std::vector<std::string> tokens = split((line.str()), '\n', true);

	return parsing(tokens);
}

bool isValidChunk(std::string const &str)
{
    size_t i = 0;
    std::string hex;

    while (isxdigit(str[i]))
        hex += str[i++];
    if (i == 0)
        return false;
    if (str[i] == '\r' and str[i + 1] == '\n')
        i += 2;
	else if (str[i] == '\n')
		i++;
    else
        return false;
    std::string s = str.substr(i);
	if (s.size() >= std::stoull(hex, nullptr, 16) + 2)
		return true;
    return false;
}

bool Request::parsingBody(std::stringstream &buffer)
{
	if ( this->isChunked )
	{
		if (not isValidChunk(this->_chunkedBody.str()))
			return false;
		else
		{
			if (parsingChunked(this->_chunkedBody) == false)
				return false;
			else
				return true;
		}
	}
	else
	{
		int WR_size = write(this->file.WR_fd, buffer.str().c_str(), buffer.str().size());
		_bodySize += WR_size;
		if (_bodySize == (off_t)this->contentLength)
			this->state = DONE;
		else if (_bodySize > (off_t)this->contentLength)
		{
			this->status = BAD_REQUEST;
			this->state = ERR;
			return false;
		}
	}
	return true;
}

void Request::setUri(std::string uri)
{
	this->_uri = uri;
}

void Request::setServer(Server *server)
{
	this->_server = server;
}

int x = 0;
bool Request::mainParsingRequest(std::stringstream &ss)
{
	if ( this->state == BODY )
	{
		std::string s = this->_chunkedBody.str();
		s.append(ss.str());
		this->_chunkedBody.str("");
		this->_chunkedBody.clear();
		this->_chunkedBody << s;
		if (x == 0)
		{
			x++;
			checkHeaders(this->_headers);
			if (this->status == BAD_REQUEST)
				return false;
		}
		return parsingBody(ss);
	}
	else if ( ss.str().find("\n") == std::string::npos )
	{
		this->_request << ss.str();
		return false;
	}
	else
		return ( readLine(ss) );
}

int checkCRLF(std::string const & str, size_t i)
{
	if (str.size() >= i + 2 and str[i] == '\r' and str[i + 1] == '\n')
		return 2;
	else if (str.size() >= i + 1 and str[i] == '\n')
		return 1;
	else
		return 0;
}

bool Request::parsingChunked(std::stringstream &buffer)
{
	size_t pos = 0;
	this->chunkSize = std::stoull(buffer.str(), nullptr, 16);

	if (chunkSize == 0)
	{
		this->state = DONE;
		return true;
	}
	pos = buffer.str().find_first_of(CRLF, pos) + checkCRLF(buffer.str(), buffer.str().find_first_of(CRLF, pos));

	int WR_size = write(this->file.WR_fd, buffer.str().substr(pos, chunkSize).c_str(), chunkSize);
	_bodySize += WR_size;

	if ( checkCRLF(buffer.str(), pos + chunkSize) == 0 )
	{
		this->status = BAD_REQUEST;
		this->state = ERR;
		return false;
	}
	pos += chunkSize + checkCRLF(buffer.str(), pos + chunkSize);


	buffer.str(buffer.str().substr(pos));
	if (isValidChunk(buffer.str()))
		return parsingChunked(buffer);
	else
		return false;
}