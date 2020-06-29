#include <fstream>
#include <cstdio>
#include <iostream>

#include "Tools/fpga_utils.hpp"

bool aff3ct::tools::write_to_device(FILE* fs, void* buffer, size_t size, size_t count, uint64_t address)
{
	int rc = 0;
	// Setting address with an offset in the file stream
	rc = fseek(fs, address, SEEK_SET);
	if (rc != 0){
		perror("Error seeking address");
		exit(EXIT_FAILURE); //TODO HANDLE
	}

	//Writing to the file
	rc = fwrite(buffer, size, count, fs);
	if (rc != count){
		perror("Error writing to channel");
		exit(EXIT_FAILURE); //TODO HANDLE
	}

	// Rewinding or it causes issue
	rc = fseek(fs, 0x0, SEEK_SET);

	return(true);
}

bool aff3ct::tools::read_from_device(FILE* fs, void* buffer, size_t size, size_t count, uint64_t address)
{   
	int rc = 0;
	// Setting address with an offset in the file stream
	rc = fseek(fs, address, SEEK_SET);
	if (rc != 0){
		perror("Error seeking address");
		exit(EXIT_FAILURE); //TODO HANDLE
	}

	// Reading from the file
	rc = fread(buffer, size, count, fs) ;
	if (rc != count){
		perror("Error reading from channel");
		exit(EXIT_FAILURE); //TODO HANDLE
	}

	// Rewinding or it causes issue
	rc = fseek(fs, 0x0, SEEK_SET);

	return(true);
}
