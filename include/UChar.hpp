/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UChar.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moseddik <moseddik@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 12:59:55 by moseddik          #+#    #+#             */
/*   Updated: 2023/06/12 20:12:53 by moseddik         ###   ########.fr       */
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

		template < typename Iterator >
		UChar( Iterator first, Iterator last )
		{
			while (first != last)
			{
				this->_buffer.push_back(*first);
				first++;
			}
			this->_size = this->_buffer.size();
		}

		~UChar( void );

		UChar & operator=( const UChar & rhs );
		UChar & operator=( const std::string & rhs );
		UChar & operator+=( const UChar & rhs );
		UChar & operator+=( const char * rhs );
		UChar & operator+=( const std::string & rhs );

		void	push_front( std::string & str );
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

		void	setBuffer( std::string & str );
};
std::ostream &operator<<(std::ostream &o, const UChar *rhs);