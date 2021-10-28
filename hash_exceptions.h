#pragma once
#include <exception>


struct KeyError : public std::exception
{
	const char * what () const throw ()
    {
    	return "Key Not Found";
    }
};
