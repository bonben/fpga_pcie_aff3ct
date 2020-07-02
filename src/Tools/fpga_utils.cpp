#include <cstdio>
#include <cstring>
#include <sstream>

#include "Tools/fpga_utils.hpp"
#include "Tools/Exception/exception.hpp"

void aff3ct::tools::write_to_device(FILE* fs, void* buffer, size_t size, size_t count, uint64_t address)
{
	int rc = 0;
	// Setting address with an offset in the file stream
	rc = fseek(fs, address, SEEK_SET);
	if (rc != 0){
		std::stringstream message;
		message << "Error seeking address " << address << " : " << std::strerror(errno);
		throw tools::io_error(__FILE__, __LINE__, __func__, message.str());
	}

	// Writing to the file
	rc = fwrite(buffer, size, count, fs);
	if (rc != count){
		std::stringstream message;
		message << "Error writing : " << std::strerror(errno);
		throw tools::io_error(__FILE__, __LINE__, __func__, message.str());
	}

	// Rewinding or it causes issue
	rc = fseek(fs, 0x0, SEEK_SET);
}

void aff3ct::tools::read_from_device(FILE* fs, void* buffer, size_t size, size_t count, uint64_t address)
{   
	int rc = 0;
	// Setting address with an offset in the file stream
	rc = fseek(fs, address, SEEK_SET);
	if (rc != 0){
		std::stringstream message;
		message << "Error seeking address " << address << " : " << std::strerror(errno);
		throw tools::io_error(__FILE__, __LINE__, __func__, message.str());
	}

	// Reading from the file
	rc = fread(buffer, size, count, fs) ;
	if (rc != count){
		std::stringstream message;
		message << "Error reading : " << std::strerror(errno);
		throw tools::io_error(__FILE__, __LINE__, __func__, message.str());
	}

	// Rewinding or it causes issue
	rc = fseek(fs, 0x0, SEEK_SET);
}
