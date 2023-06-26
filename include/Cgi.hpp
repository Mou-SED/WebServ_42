/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 15:08:37 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/06/26 00:56:04 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include "Request.hpp"
#include <signal.h>


enum eState
{
	stateStar = 1 << 0,
	stateRequestLine = 1 << 1,
	stateBeforeHeader = 1 << 2,
	stateHeader = 1 << 3,
	stateNewline = 1 << 4,
	stateBody = 1 << 5,
	stateEnd = 1 << 6,
};
#define SERVER_READ 0
#define CGI_WRITE 1
#define CGI_READ 2
#define SERVER_WRITE 3

class Request;


class Cgi{
	private:
		std::string							_path;
		std::string							_method;
		std::map<std::string, std::string>	_headers;
		std::vector<std::string> 			_cookies;
		File								_fileBody;
		std::string							_response;
		std::string							_app;
		std::string							_query;
		std::vector<std::string>			_env;
		uint16_t							_status;
		off_t								_sizeBody;
		std::string							_body;
		std::string 						_location;
		std::string 						_contentType;
		std::string 						_setCookie;

	public:
		Cgi(void);
		~Cgi(void);
		Cgi(Cgi const & src);
		Cgi(std::string const & path, std::string const & method, Request & req, std::pair<std::string, Directives > * location );
		Cgi & operator=(Cgi const & rhs);
		eState state;
		off_t 	bodySize;
		int 	execute(void);
		bool		foundBody;
		std::vector<int> fds = std::vector<int>(4);
		int 		pid;
		int 		ret;
		int			exStatus;
		void	sendingBody(void);
		void	childProcess(void);
		void	parentProcess(void);
		void	setEnv(void);
		void 	setApp(std::string const & app);
		void 	setPath(std::string const & path);
		void 	setMethod(std::string & method);
		void 	setBody(std::string & body);
		void 	setQuery(std::string & query);
		void	setHeaders(std::map<std::string, std::string>& headers);
		void	addHeader(std::string & key, std::string & value);

		void	addToHeaders(std::string &str);
		off_t	sizeFile( void );
		void	createPipe(void);

		void	waitingChild(void);

		bool 	checkApp(const char *env_var);
		bool 	checkPath(std::string & path);

		std::string getPath(void) const;
		std::string getMethod(void) const;
		std::string getHeaders(void) const;
		std::string getHeaderString(void) const;
		std::string getBody(void) const;
		std::string getResponse(void) const;
		std::string getQuery(std::string const &path) const;
		uint16_t 	getStatus(void) const;
		off_t 		getSizeBody(void) const;

		std::string getCookie(void) const;
		std::string getLocation(void) const;
		std::string getContentType(void) const;


		void generateResponse(void);
		void clear(void);

};