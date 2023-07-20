/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 15:08:37 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/07/20 10:50:56 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include "Request.hpp"
#include <signal.h>
#include <string>
#include <unistd.h>
#include <vector>

#define SERVER_WRITE 3
#define SERVER_READ 0
#define CGI_WRITE 1
#define CGI_READ 2

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

class Request;
class Cgi {
	private:
		std::map<std::string, std::string>	_headers;
		std::vector<std::string> 			_cookies;
		std::vector<std::string>			_env;
		std::string 						_contentType;
		std::string 						_setCookie;
		std::string							_response;
		std::string 						_location;
		std::string							_method;
		std::string							_query;
		std::string							_path;
		std::string							_body;
		std::string							_app;
		uint16_t							_status;
		off_t								_sizeBody;
		File								_fileBody;

	public:
		Cgi(void);
		~Cgi(void);
		Cgi(Cgi const & src);
		Cgi(std::string const &path, std::string const &method, Request &req, std::pair<std::string, Directives > * location, int cgi_passIndex );
		Cgi & operator=(Cgi const & rhs);

		eState 		state;
		off_t 		bodySize;
		int 		execute(void);
		bool		foundBody;
		std::vector<int> fds;
		int 		pid;
		int 		ret;
		int			exStatus;
		void		setHeaders(std::map<std::string, std::string>& headers);
		void		addHeader(std::string & key, std::string & value);
		void 		setPath(std::string const & path);
		void		setFds(std::string const & methode);
		void 		setApp(std::string const & app);
		void 		setMethod(std::string & method);
		void 		setQuery(std::string & query);
		void 		setBody(std::string & body);
		void		parentProcess(void);
		void		childProcess(void);
		void		sendingBody(void);
		void		setEnv(void);

		void		addToHeaders(std::string &str, bool &body);
		off_t		sizeFile( void );
		void		createPipe(void);

		void		waitingChild(void);

		bool 		checkApp(const char *env_var);
		bool 		checkPath(std::string & path);

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


		void 		generateResponse(void);
		void 		clear(void);

};
