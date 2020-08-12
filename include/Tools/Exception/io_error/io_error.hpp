/*!
 * \file
 * \brief Class tools::io_error.
 */
#ifndef IO_ERROR_HPP_
#define IO_ERROR_HPP_

#include <string>

#include "Tools/Exception/exception.hpp"

namespace aff3ct
{
namespace tools
{
class io_error : public exception
{
	static const std::string default_message;

public:
	io_error() throw();

	explicit io_error(const std::string &message) throw();

	io_error(const std::string &filename,
	                    const int line_num,
	                    const std::string &funcname = exception::empty_string,
	                    const std::string &message = exception::empty_string) throw();

	virtual ~io_error() throw() = default;
};
}
}

#endif /* IO_ERROR_HPP_ */
