/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Check.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/30 16:29:15 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/05/09 14:20:36 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <set>
#include <string>
#include <stack>
#include "WebServ.hpp"


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