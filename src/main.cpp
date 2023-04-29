/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 21:45:12 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/04/29 15:06:26 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

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
