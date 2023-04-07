/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 22:02:58 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/04/07 15:59:54 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
bool	webserv(const char *av)
{
	std::string s(av);
	std::ifstream file(s);

	if (not file.is_open() or not parcing(file, s))
		return (write(2, "Error\n", 6), false);
	return (write(1, "OK!\n", 4), true);
}