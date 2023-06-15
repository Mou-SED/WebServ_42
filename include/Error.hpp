/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/22 13:03:04 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/06/15 17:10:09 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "WebServ.hpp"

enum ERRORS {
		CONTINUE              	  = 100,
		SWITCHING_PROTOCOLS   	  = 101,
		PROCESSING            	  = 102,
		OK                    	  = 200,
		CREATED               	  = 201,
		ACCEPTED              	  = 202,
		NO_CONTENT            	  = 204,
		PARTIAL_CONTENT       	  = 206,
		SPECIAL_RESPONSE      	  = 300,
		MOVED_PERMANENTLY     	  = 301,
		MOVED_TEMPORARILY     	  = 302,
		SEE_OTHER             	  = 303,
		NOT_MODIFIED          	  = 304,
		TEMPORARY_REDIRECT    	  = 307,
		PERMANENT_REDIRECT    	  = 308,
		BAD_REQUEST           	  = 400,
		UNAUTHORIZED          	  = 401,
		FORBIDDEN             	  = 403,
		NOT_FOUND             	  = 404,
		METHOD_NOT_ALLOWED    	  = 405,
		REQUEST_TIME_OUT      	  = 408,
		CONFLICT              	  = 409,
		LENGTH_REQUIRED       	  = 411,
		PRECONDITION_FAILED   	  = 412,
		REQUEST_ENTITY_TOO_LARGE  = 413,
		REQUEST_URI_TOO_LARGE     = 414,
		UNSUPPORTED_MEDIA_TYPE    = 415,
		RANGE_NOT_SATISFIABLE     = 416,
		TOO_MANY_REQUESTS         = 429,
		INTERNAL_SERVER_ERROR     = 500,
		NOT_IMPLEMENTED           = 501,
		BAD_GATEWAY               = 502,
		SERVICE_UNAVAILABLE       = 503,
		GATEWAY_TIME_OUT          = 504,
		VERSION_NOT_SUPPORTED     = 505
};

class Error
{
	private:
		uint16_t 	_error;
		std::string _errorPage;
		std::string _errorMessage;
		std::string _errorBody;
		// off_t		_errorBodySize;

	public:
		Error( uint16_t error );
		Error(uint16_t error, std::vector<std::pair<std::set<int>, std::string> > error_page);
		Error(uint16_t error, std::string error_page);
		Error(const Error &src);
		~Error();
		void setErrorMessage(std::string const & msg);
		std::string getErrorMessage(void) const;
		std::string getErrorBody(void) const;
};