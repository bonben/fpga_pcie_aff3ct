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

bool write_to_device(const char*, FILE*, const void*, uint64_t, int, uint64_t);
bool read_from_device(const char*, FILE*, void*, uint64_t, int, uint64_t);

}
}

#endif /* FPGA_UTILS_H_*/