/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/07 22:21:18 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/04/07 22:21:19 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <stdexcept>

bool	webserv(const char *av);
bool	parcing(std::ifstream &file, std::string s);
bool	ipAddress(std::string s);

