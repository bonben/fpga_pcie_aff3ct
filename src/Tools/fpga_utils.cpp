#include <fstream>
#include <cstdio>
#include <iostream>

#include "Tools/fpga_utils.hpp"

bool aff3ct::tools::write_to_device(const char* fname, FILE* fs, const void* buffer, uint64_t size, int count, uint64_t address)
{
    // Setting address with an offset in the file stream
    if (fseek(fs, address, SEEK_SET)){
        std::cerr << fname << " (write) : error setting offset for address " << address << std::endl;
        exit(EXIT_FAILURE);
    }
    // Writing to the file
    if (fwrite(buffer, size, count, fs) != count){
        std::cerr << fname << " : error writing" << std::endl;
        exit(EXIT_FAILURE);
    }
    return(true);
}

bool aff3ct::tools::read_from_device(const char* fname, FILE* fs, void* buffer, uint64_t size, int count, uint64_t address)
{
    // Setting address with an offset in the file stream
    if (fseek(fs, address, SEEK_SET)){
        std::cerr << fname << " (read) :error setting offset for address " << address << std::endl;
        exit(EXIT_FAILURE);
    }
    // Reading from the file
    if (fread(buffer, size, count, fs) != count){
        std::cerr << fname << " : error reading" << std::endl;
        exit(EXIT_FAILURE);
    }
    return(true);
}
