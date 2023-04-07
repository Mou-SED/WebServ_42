/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parcing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 22:23:43 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/04/07 20:38:47 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

bool ipAddress(std::string s)
{
	std::string buff("");
	std::vector<std::string> v;
	std::stringstream ss;
	ss << s;
	while (getline(ss, buff, '.'))
	{
		if ( strspn(buff.c_str(), "0123456789") == buff.size() and buff != "" )
		{
			int x = atoi(buff.c_str());
			if (x < 0 || x > 255)
				return false;
			v.push_back(buff);
		}
		else
			return false;
	}
	if (v.size() != 4)
		return false;
	return true;
}

bool parcing(std::ifstream &file, std::string s)
{
	if (not file.is_open())
		return (false);
	(void)s;
	return (true);
}