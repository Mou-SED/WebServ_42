/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UChar.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 12:59:55 by moseddik          #+#    #+#             */
/*   Updated: 2023/05/23 18:33:22 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "WebServ.hpp"

class UChar
{
	private:
		std::vector<unsigned char> _buffer;
		size_t _size;
	public:
		UChar( void );
		UChar( size_t size );
		UChar(char *buffer);
		UChar( std::string & str );
		UChar( const UChar & src );
		~UChar( void );
		UChar & operator=( const UChar & rhs );
		UChar & operator=( const std::string & rhs );
		UChar & operator+=( const UChar & rhs );
		UChar & operator+=( const char * rhs );
		UChar & operator+=( const std::string & rhs );
		void	push_front( std::string & str );
		std::vector<unsigned char>  getUChar( void );
		unsigned char & operator[]( int index );
		size_t size( void ) const;
		void resize( int size );
		void append( unsigned char c );
		void	append(std::string & buffer);
		void	append(UChar &buffer);
		void clear( void );
		std::string toString( void ) const;
		std::vector<unsigned char> & getBuffer( void );
		bool	empty( void ) const;
		bool	endWith( std::string & str ) const;
};
std::ostream &operator<<(std::ostream &o, const UChar *rhs);