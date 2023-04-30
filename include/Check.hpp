/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Check.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/30 16:29:15 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/04/30 20:37:49 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <set>
#include <string>
#include <stack>
#include "webserv.hpp"


struct Check
{
	static int  num_line;
	static std::set<std::string> methods;
	static ::std::stack<char> brackets;
	static std::string key;
	static std::string value;
	static bool ipAddress(std::string s);
	static bool isPort( std::string s );
	static bool isDomain( std::string const & s );
};