/*!
 * \file
 * \brief Class tools::exception_fpga.
 */
#ifndef EXCEPTION_FPGA_HPP_
#define EXCEPTION_FPGA_HPP_

#include <exception>
#include <string>

namespace aff3ct
{
namespace tools
{
class exception_fpga : public std::exception
{
public:
	static bool no_backtrace;
	static bool no_addr_to_line;

protected:
	static const std::string empty_string;

private:
	std::string message;   // the message only
#ifdef AFF3CT_BACKTRACE
	std::string backtrace; // the message + the backtrace
	std::string backtrace_a2l; // the message + the backtrace with addr_to_line conversion
#endif

public:
	exception_fpga() throw();

	explicit exception_fpga(const std::string &message) throw();

	exception_fpga(const std::string &filename,
	          const int line_num,
	          const std::string &funcname,
	          const std::string &message) throw();

	virtual ~exception_fpga() throw() = default;

	virtual const char* what() const throw(); // return the message and the back trace if enabled
};
}
}

// include specific exceptions

#include "io_error/io_error.hpp"

#endif /* EXCEPTION_FPGA_HPP_ */
