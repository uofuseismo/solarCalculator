# Building the Software

## Prerequisites

   1. A C++20 compliant compiler.
   2. CMake v 3.16 or greater.
   3. UUSS's Time [library](https://github.com/uofuseismo/time).

## Optional

   1. Python3 and [pybind11](https://pybind11.readthedocs.io/en/stable/) for building the Python bindings.

## Configuring

A simple configuration script that builds the base library and the Python library may look like

    #!/bin/bash
    export CXX=clang++
    export BUILD_DIR=clang_build
    if [ -d ${BUILD_DIR} ]; then
       rm -rf ${BUILD_DIR}
    fi
    mkdir ${BUILD_DIR}
    cd ${BUILD_DIR}
    cmake .. \
    -DCMAKE_CXX_COMPILER=${CXX} \
    -DCMAKE_BUILD_TYPE=Release \
    -DWRAP_PYTHON=ON \
    -DPYTHON_EXECUTABLE=${HOME}/anaconda3/bin/python \
    -DPYTHON_LIBRARIES=${HOME}/anaconda3/lib

Alternatively, you may simply want to 
