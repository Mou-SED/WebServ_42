#include "Error.hpp"

Error::Error()
{
	this->error = OK;
	return;
}

Error::Error(ERRORS error)
{
	this->error = error;
	return;
}

Error::Error(ERRORS error, std::string error_page)
{
	this->error = error;
	this->error_page = error_page;
	return;
}

Error::Error(const Error &src)
{
	*this = src;
	return;
}

Error::~Error()
{
	return;
}

// std::string	genetatErrorPage( void )
// {
// 	if (error >= 400 and error <= 500)
// 	{
// 		// clientError();
// 		error_message = "Client Error";
// 		error_code = "4xx";
// 	}
// 	else if (error >= 500 and error < 506)
// 	{
// 		// serverError();
// 		error_message = "Server Error";
// 		error_code = "5xx";
// 	}
// 	else
// 	{
// 		error_message = "Unknown";
// 		error_code = "xxx";
// 	}
// }
