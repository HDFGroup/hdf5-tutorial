#include "hdf5.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    unsigned majnum, minnum, relnum;
    H5get_libversion(&majnum, &minnum, &relnum);
    printf("Hello, HDF5 library %d.%d.%d!", majnum, minnum, relnum);

    // If the user provided a filename, create a file under that name
    // otherwise, create a file called "build/hello.h5"
    hid_t file = H5Fcreate((argc > 1 ? argv[1] : "build/hello.h5"), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    H5Fclose(file);

    return 0;
}
