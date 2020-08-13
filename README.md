# FPGA PCIe AFF3CT

The goal of this project is to provide a way of running hardware implementation as part ofan AFF3CT communication chain : for example, we can imagine a whole communication chainrunning in AFF3CT with a LDPC coder/decoder running on an external FPGA, or even more :running the complete communication chain on hardware. It’d bring out even more possibilities toAFF3CT as a testing and development tool, through hardware performance measurement,functional debugging or hardware prototyping.This project aims to provide a software/hardware interface between AFF3CT and a FPGAimplementation   of   any   part   of   a   communication   chain.  We   use   PCIe   as   a   support   forsoftware/hardware communication, as it’s very fast and very popular through hardware peripherals. 

To build :

git submodule init lib/aff3ct	
git submodule update --init --recursive
mkdir build
cd build
cmake .. -G"Unix Makefiles" -DCMAKE_CXX_COMPILER="g++" -DCMAKE_BUILD_TYPE="Release" -DCMAKE_CXX_FLAGS="-funroll-loops -march=native"
make -j4

To remove AFF3CT (for size conveniency) :

git submodule deinit lib/aff3ct
rm -r build/lib/aff3ct
rm -r .git/modules/lib/aff3ct

