/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 15:44:45 by moseddik          #+#    #+#             */
/*   Updated: 2023/06/12 13:30:24 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

std::vector<std::string> split(std::string &str, char delim, bool keepDelim);
bool	isDirectory( const std::string & path );
bool	isFile( const std::string & path );
bool	beginsWith( const std::string & str, const std::string & prefix );