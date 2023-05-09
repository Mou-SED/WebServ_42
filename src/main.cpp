/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 21:45:12 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/05/09 14:20:55 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

int main(int ac, char **av)
{
	if (ac > 2)
		return (write(2, "Error: Multi Argument [..]\n", 6), 1);
	try
	{
		if (ac == 1)
			webserv("conf/webserv.conf");
		else
			webserv(av[1]);
	}
	catch(const std::exception & e)
	{
		std::cerr << e.what() << '\n';
	}
}
