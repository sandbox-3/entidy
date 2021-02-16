#pragma once
#include <exception>
#include <string>

namespace entidy
{
class EntidyException : virtual public std::exception
{
protected:
	std::string error_message;

public:
	explicit EntidyException(const std::string& msg)
		: error_message(msg)
	{ }
	virtual ~EntidyException() throw() { }
	virtual const char* what() const throw()
	{
		return error_message.c_str();
	}
};
} // namespace entidy