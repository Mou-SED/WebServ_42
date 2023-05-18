/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UChar.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 12:59:55 by moseddik          #+#    #+#             */
/*   Updated: 2023/05/18 17:14:01 by moseddik         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "WebServ.hpp"

class UChar
{
	private:
		std::vector<unsigned char> _buffer;
		int _size;
	public:
		UChar( void );
		UChar( int size );
		UChar( std::string & str );
		UChar( const UChar & src );
		~UChar( void );
		UChar & operator=( const UChar & rhs );
		UChar & operator=( const std::string & rhs );
		UChar & operator+=( const UChar & rhs );
		UChar & operator+=( const char * rhs );
		UChar & operator+=( const std::string & rhs );
		unsigned char & operator[]( int index );
		int size( void ) const;
		void resize( int size );
		void append( unsigned char c );
		void	append(std::string & buffer);
		void clear( void );
		std::string toString( void ) const;
		std::vector<unsigned char> & getBuffer( void );
		bool	empty( void ) const;
		bool	endWith( std::string & str ) const;
};
std::ostream &operator<<(std::ostream &o, const UChar *rhs);