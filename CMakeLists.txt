cmake_minimum_required(VERSION 3.14)

project(Tutorial)

# fetch latest argparse
include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

#find packages
find_package(MPI REQUIRED)
find_package(HDF5 REQUIRED COMPONENTS C)

add_executable(hdf-tutorial-mpi hdf-tutorial-mpi.cpp)

target_link_libraries(hdf-tutorial-mpi PRIVATE HDF5::HDF5 MPI::MPI_C)

include_directories(${MPI_INCLUDE_PATH} ${HDF5_INCLUDE_DIRS} )