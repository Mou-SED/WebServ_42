/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 22:02:58 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/05/09 14:22:27 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

bool	webserv(const char *av)
{
	std::string s(av);
	std::ifstream file(s);
	if (not file.is_open() or not parcing(file, s))
		throw std::runtime_error("Error");
	return (true);
}