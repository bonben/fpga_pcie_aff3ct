cmake_minimum_required (VERSION 3.2)
cmake_policy(SET CMP0054 NEW)

project (fpga_pcie_aff3ct)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify bin path
set (EXECUTABLE_OUTPUT_PATH bin/)

# Generate the source files lists
set(SRC_FILES_LIB ${CMAKE_CURRENT_SOURCE_DIR}/src/Factory/Module/FPGA/FPGA.cpp)
set(SRC_FILES_MAIN ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

# add library & executable
add_library   (fpga_pcie_aff3ct_lib STATIC ${SRC_FILES_LIB})
add_executable(fpga_pcie_aff3ct            ${SRC_FILES_MAIN})

target_include_directories(fpga_pcie_aff3ct_lib PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>)
target_include_directories(fpga_pcie_aff3ct     PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>)

# Configure the AFF3CT library before compiling it
option(AFF3CT_COMPILE_EXE        ""      OFF            )
option(AFF3CT_COMPILE_STATIC_LIB ""      ON             )
option(AFF3CT_COMPILE_SHARED_LIB ""      OFF            )
option(AFF3CT_LINK_GSL           ""      OFF            )
option(AFF3CT_LINK_MKL           ""      OFF            )
set   (AFF3CT_EXT_STRINGS        "OFF"                  )
set   (AFF3CT_PREC               "MULTI" CACHE STRING "") # can be '8', '16', '32', '64' or 'MULTI'

# Compile the AFF3CT library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/aff3ct/)

# Link with AFF3CT
target_link_libraries(fpga_pcie_aff3ct     PRIVATE aff3ct-static-lib)
target_link_libraries(fpga_pcie_aff3ct_lib PRIVATE aff3ct-static-lib)
target_link_libraries(fpga_pcie_aff3ct     PRIVATE fpga_pcie_aff3ct_lib)
