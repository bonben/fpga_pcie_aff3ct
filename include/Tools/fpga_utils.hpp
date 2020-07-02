/*!
 * \file
 * \brief Functions for FPGA read/write operations.
 */

#ifndef FPGA_UTILS_H
#define FPGA_UTILS_H

#include <fstream>
#include <cstdio>

namespace aff3ct
{
namespace tools
{

void write_to_device  (FILE*, void*, size_t, size_t, uint64_t);
void read_from_device (FILE*, void*, size_t, size_t, uint64_t);

}
}

#endif /* FPGA_UTILS_H_*/