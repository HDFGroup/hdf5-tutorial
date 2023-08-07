cmake_minimum_required(VERSION 3.10)

project(Tutorial)

find_package(MPI REQUIRED)
find_package(HDF5 REQUIRED COMPONENTS C)

#add_executable(hdf-tutorial-mpi hdf-tutorial-mpi.cpp)
add_executable(hdf-tutorial hdf-tutorial.cpp)

#target_link_libraries(hdf-tutorial-mpi PRIVATE HDF5::HDF5 MPI::MPI_C)
target_link_libraries(hdf-tutorial PRIVATE HDF5::HDF5)

include_directories(${MPI_INCLUDE_PATH} ${HDF5_INCLUDE_DIRS} )
include_directories(${HDF5_INCLUDE_DIRS} )
