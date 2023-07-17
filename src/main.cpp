/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: junik <abderrachidyassir@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 21:45:12 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/07/17 06:46:57 by junik            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/WebServ.hpp"

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
