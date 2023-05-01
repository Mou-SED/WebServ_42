/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenization.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 18:37:58 by aaggoujj          #+#    #+#             */
/*   Updated: 2023/05/01 18:50:02 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

enum Tokens
{
	TOKEN_NONE,
	TOKEN_LEFT_BRACKET,
	TOKEN_RIGHT_BRACKET,
	TOKEN_SERVER,
	TOKEN_LOCATION,
	TOKEN_SEMICOLON,
	TOKEN_NAME,
	TOKEN_VALUE,
	TOKEN_INITIALIZED,
};


struct ASTNode
{
	Tokens token;
	std::string value;
	ASTNode* left;
	ASTNode* right;
	ASTNode()
	{
		token=TOKEN_NONE;
		value="";
		left=nullptr;
		right=nullptr;
	}
	~ASTNode()
	{
		delete left;
		delete right;
	}
};


void	Tokenization(std::string const &key, std::string const &value, ASTNode *root);
