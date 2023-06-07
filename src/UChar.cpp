/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UChar.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aaggoujj <aaggoujj@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 13:08:48 by moseddik          #+#    #+#             */
/*   Updated: 2023/05/23 18:33:32 by aaggoujj         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "UChar.hpp"

UChar::UChar(void)
{
	this->_size = 0;
	return ;
}

UChar::UChar(size_t size)
{
	this->_size = size;
	this->_buffer.resize(size);
	return ;
}

UChar::UChar(char *buffer)
{
	this->_size = strlen(buffer);
	this->_buffer.resize(this->_size);
	for (size_t i = 0; i < this->_size; i++)
		this->_buffer[i] = buffer[i];
	return ;
}

UChar::UChar(std::string &str)
{
	this->_size = str.size();
	this->_buffer.resize(this->_size);
	for (size_t i = 0; i < this->_size; i++)
		this->_buffer[i] = str[i];
	return ;
}

UChar::UChar(const UChar &src)
{
	*this = src;
	return ;
}

UChar::~UChar(void)
{
	return ;
}

UChar &UChar::operator=(const UChar &rhs)
{
	if (this != &rhs)
	{
		this->_size = rhs._size;
		this->_buffer = rhs._buffer;
	}
	return *this;
}

UChar &UChar::operator=(const std::string &rhs)
{
	this->_size = rhs.size();
	this->_buffer.resize(this->_size);
	for (size_t i = 0; i < this->_size; i++)
		this->_buffer[i] = rhs[i];
	return *this;
}

UChar &UChar::operator+=(const UChar &rhs)
{
	this->_buffer.insert(this->_buffer.end(), rhs._buffer.begin(), rhs._buffer.end());
	this->_size += rhs._size;
	return *this;
}

UChar &UChar::operator+=(const char *rhs)
{
	int i = 0;
	while (rhs[i])
	{
		this->_buffer.push_back(rhs[i]);
		i++;
	}
	this->_size += i;
	return *this;
}

UChar &UChar::operator+=(const std::string &rhs)
{
	this->_buffer.insert(this->_buffer.end(), rhs.begin(), rhs.end());
	this->_size += rhs.size();
	return *this;
}

void UChar::push_front(std::string &str)
{
	this->_buffer.insert(this->_buffer.begin(), str.begin(), str.end());
	this->_size += str.size();
	return ;
}

std::vector<unsigned char> UChar::getUChar(void)
{
	return this->_buffer;
}

unsigned char &UChar::operator[](int index)
{
	return this->_buffer[index];
}

size_t UChar::size(void) const
{
	return this->_size;
}

void UChar::resize(int size)
{
	this->_size = size;
	this->_buffer.resize(size);
	return ;
}

void UChar::append(unsigned char c)
{
	this->_buffer.push_back(c);
	this->_size++;
	return ;
}

void UChar::append(std::string &buffer)
{
	this->_buffer.insert(this->_buffer.end(), buffer.begin(), buffer.end());
	this->_size += buffer.size();
	return ;
}

void UChar::append(UChar &buffer)
{
	this->_buffer.insert(this->_buffer.end(), buffer._buffer.begin(), buffer._buffer.end());
	this->_size += buffer._size;
	return ;
}

void UChar::clear(void)
{
	this->_buffer.clear();
	this->_size = 0;
	return ;
}

std::string UChar::toString(void) const
{
	std::string str(this->_size, 0);
	for (size_t i = 0; i < this->_size; i++)
		str[i] = this->_buffer[i];
	return str;
}

std::vector<unsigned char> &UChar::getBuffer(void)
{
	return this->_buffer;
}

bool UChar::empty(void) const
{
	return this->_buffer.empty();
}

bool UChar::endWith(std::string &str) const
{
	if (this->_size < str.size())
		return false;
	for (size_t i = 0; i < str.size(); i++)
		if (this->_buffer[this->_size - str.size() + i] != str[i])
			return false;
	return true;
}

std::ostream &operator<<(std::ostream &o, const UChar &rhs)
{
	o << rhs.toString();
	return o;
}
