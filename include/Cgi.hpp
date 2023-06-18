/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 15:08:37 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/06/18 17:37:23 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

/*
{"%20", "Space"},
{"%21", "!"},
{"%22", """},
{"%23", "#"},
{"%24", "$"},
{"%25", "%"},
{"%26", "&"},
{"%27", "'"},
{"%28", "("},
{"%29", ")"},
{"%2A", "*"},
{"%2B", "+"},
{"%2C", ","},
{"%2D", "-"},
{"%2E", "."},
{"%2F", "/"},
{"%3A", ":"},
{"%3B", ";"},
{"%3C", "<"},
{"%3D", "="},
{"%3E", ">"},
{"%3F", "?"},
{"%40", "@"},
{"%5B", "["},
{"%5C", "\"},
{"%5D", "]"},
{"%5E", "^"},
{"%5F", "_"},
{"%60", "`"},
{"%7B", "{"},
{"%7C", "|"},
{"%7D", "}"},
{"%7E", "~"}
*/

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

class Cgi{
	private:
		std::string _path;
		std::string _method;
		std::map<std::string, std::string> _headers;
		std::string _body;
		std::string _response;
		std::string _app;
		std::map<std::string, std::string> _query;
	public:
		Cgi(void);
		~Cgi(void);
		Cgi(Cgi const & src);
		Cgi & operator=(Cgi const & rhs);
		eState state;
		off_t 	bodySize;
		int 	execute(void);
		int 	fds[2];
		int 	pid;
		int 	status;
		int 	ret;

		void	childProcess(void);
		void	parentProcess(void);
	
		void addQuery(std::string & key, std::string & value);
		void setApp(std::string const & app);
		void setPath(std::string const & path);
		void setMethod(std::string & method);
		void setBody(std::string & body);
		void	setHeaders(std::map<std::string, std::string>& headers);
		void	addHeader(std::string & key, std::string & value);

		bool 	checkApp(const char *env_var);
		bool 	checkPath(std::string & path);

		std::string getPath(void) const;
		std::string getMethod(void) const;
		std::map<std::string, std::string> getHeaders(void) const;
		std::string getBody(void) const;
		std::string getResponse(void) const;
		std::map<std::string, std::string> getQuery(void) const;
		std::string getValueQuery(std::string const & key) const;

		void	creatQuery(std::string &query);
		void	decode(std::string &str);

		void generateResponse(void);
		void clear(void);

};