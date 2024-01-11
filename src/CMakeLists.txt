# fetch latest argparse
#include(FetchContent)
#FetchContent_Declare(
#    argparse
#    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
#)
#FetchContent_MakeAvailable(argparse)

#find packages
find_package(MPI REQUIRED)
find_package(HDF5 REQUIRED COMPONENTS C)

add_compile_options(-Wall -Wextra -pedantic -Werror)

add_executable(hello hello.c)

add_executable(hello-hdf5 hello_hdf5.cpp)
target_link_libraries(hello-hdf5 ${HDF5_C_LIBRARIES})

add_executable(ou-text ou_text.cpp ou_sampler.cpp)
set_property(TARGET ou-text PROPERTY CXX_STANDARD 17)

add_executable(ou-binary ou_binary.cpp ou_sampler.cpp)
set_property(TARGET ou-binary PROPERTY CXX_STANDARD 17)

add_executable(ou-hdf5 ou_hdf5.cpp ou_sampler.cpp docstring.cpp)
set_property(TARGET ou-hdf5 PROPERTY CXX_STANDARD 17)
target_link_libraries(ou-hdf5 ${HDF5_C_LIBRARIES})

add_executable(ou-hdf5.1 ou_hdf5.1.cpp ou_sampler.cpp docstring.cpp parse_arguments1.cpp ou_sampler1.cpp)
set_property(TARGET ou-hdf5.1 PROPERTY CXX_STANDARD 17)
target_link_libraries(ou-hdf5.1 ${HDF5_C_LIBRARIES})

#add_executable(ou-hdf5-mpi ou_hdf5_mpi.cpp parse_arguments.cpp parse_arguments2.cpp partitioner.cpp ou_sampler.cpp)
#set_property(TARGET ou-hdf5-mpi PROPERTY CXX_STANDARD 17)
#target_link_libraries(ou-hdf5-mpi PRIVATE HDF5 MPI::MPI_C)

include_directories(${MPI_INCLUDE_PATH} ${HDF5_INCLUDE_DIRS} "../include")