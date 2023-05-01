/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenization.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 18:32:41 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/05/01 20:50:01 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

Tokens get_tokens(std::string const & value)
{
	if (value == "server")
		return (TOKEN_SERVER);
	else if (value == "location")
		return (TOKEN_LOCATION);
	else if (value == "{")
		return (TOKEN_LEFT_BRACKET);
	else if (value == "}")
		return (TOKEN_RIGHT_BRACKET);
	else if (value == ";")
		return (TOKEN_SEMICOLON);
	else if (value == "name")
		return (TOKEN_NAME);
	else if (value == "value")
		return (TOKEN_VALUE);
	else
		return (TOKEN_NONE);
}

void	Tokenization(std::string const &key, std::string const &value, ASTNode *root)
{
	ASTNode *node = new ASTNode;

	(void)root;
	node->value = value;
	node->token = get_tokens(key);
	std::cout << key << " " << value << std::endl;
}