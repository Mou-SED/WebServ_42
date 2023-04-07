#pragma once

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

bool	webserv(const char *av);
bool	parcing(std::ifstream &file, std::string s);
bool	ipAddress(std::string s);
