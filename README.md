# FPGA PCIe AFF3CT

(https://aff3ct.github.io/][AFF3CT] is a toolbox dedicated to build communication chains. By assembling differentmodules, users are able to build a complete communication chain and simulate its performance.Every component is logically represented through the software

The goal of this project is to provide a way of running hardware implementation as part of an AFF3CT communication chain : for example, we can imagine a whole communication chainrunning in AFF3CT with a LDPC coder/decoder running on an external FPGA, or even more : running the complete communication chain on hardware. 
It’d bring out even more possibilities to AFF3CT as a testing and development tool, through hardware performance measurement,functional debugging or hardware prototyping.

This project aims to provide a software/hardware interface between AFF3CT and a FPGA implementation of any part of a communication chain. We use PCIe as a support for software/hardware communication, as it’s very fast and very popular through hardware peripherals. 

## Build

In order to build this project, you need the (https://github.com/Xilinx/dma_ip_drivers)[XDMA IP driver] working properly with your design. AFF3CT is included as a git submodule. AFF3CT and this project use CMake during the build phase. To build this project this CMake, run :

```
git submodule init lib/aff3ct	
git submodule update --init --recursive
mkdir build
cd build
cmake .. -G"Unix Makefiles" -DCMAKE_CXX_COMPILER="g++" -DCMAKE_BUILD_TYPE="Release" -DCMAKE_CXX_FLAGS="-funroll-loops -march=native"
make -j4
```

AFF3CT is a submodule of this project and is not tracked by git. To completly remove AFF3CT from this project (for size conveniency), run :
```
git submodule deinit lib/aff3ct
rm -r build/lib/aff3ct
rm -r .git/modules/lib/aff3ct
```

## Performance

We're able to achieve high-performance data transfer from AFF3CT to FPGA memory and from FPGA memory to AFF3CT. With a PCIe Gen3 x8 configuration, data rate goes up to 4GB/s.

![AFF3CT performance demo](https://github.com/bonben/fpga_pcie_aff3ct/edit/master/perf.jpg "AFF3CT performance demo")

In this picture, AFF3CT shows a transfer rate of about 1000 Mb/s for both `send` and `receive` transfer with 32 bits encoded data, which means an effective data rate of 32*1000 = 4 GB/s. Maximum theoretical data rate in this configuration is 7.88 GB/s __without__ AFF3CT.
