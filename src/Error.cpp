#include "Error.hpp"

Error::Error()
{
	this->error = OK;
	return;
}

Error::Error(uint16_t error)
{
	this->error = error;
	return;
}

Error::Error(uint16_t error, std::string error_page)
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

std::string const Error::errorMessage(std::string msg)
{
	std::string const s = "<html>\n<head><title>"+msg+"</title></head>\n<body>\n<center><h1>"+msg+"</h1></center>\n<hr><center>WebServ/0.1</center>\n</body>\n</html>\n";
	std::string header  = "HTTP/1.1 "+std::to_string(this->error)+" "+msg+"\r\n"+"Content-Type: text/html\r\n"+"Content-Length: "+std::to_string(s.length())+"\r\n\r\n";
	return header + s;
}

std::string	genetatErrorPage( void )
{
	// if (error >= 400 and error <= 500)
	// {
	// 	// clientError();
	// 	error_message = "Client Error";
	// 	error_code = "4xx";
	// }
	// else if (error >= 500 and error < 506)
	// {
	// 	// serverError();
	// 	error_message = "Server Error";
	// 	error_code = "5xx";
	// }
	// else
	// {
	// 	error_message = "Unknown";
	// 	error_code = "xxx";
	// }
	return "";
}
