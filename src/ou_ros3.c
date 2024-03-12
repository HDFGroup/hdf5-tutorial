#include "hdf5.h"
#include <array>
#include <cassert>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(void)
{
H5FD_ros3_fapl_t s3_params;

// Set the AWS region and the S3 bucket name
strcpy(s3_params.aws_region, "us-east-2");
string file_uri = "https://s3.us-east-2.amazonaws.com/docs.hdfgroup.org/hdf5/h5/ou_process.h5";
// The version of this ROS3 FAPL parameter structure
s3_params.version = 1;
// This file permits anonymous access, so authentication is not needed
// secret_id and secret_key auth fields that would be required for protected S3 buckets
s3_params.authenticate = 0; 

// Open and read from the file
auto fapl = H5Pcreate(H5P_FILE_ACCESS);
H5Pset_fapl_ros3(fapl, &s3_params);
auto file = H5Fopen(file_uri.c_str(), H5F_ACC_RDONLY, fapl);
auto dset = H5Dopen(file, "dataset", H5P_DEFAULT);

// Get the dataspace details from the file
auto dspace = H5Dget_space(dset);
assert(H5Sget_simple_extent_ndims(dspace) == 2);
array<hsize_t, 2> dims;
H5Sget_simple_extent_dims(dspace, dims.data(), NULL);
vector<double> read_buffer(dims[0] * dims[1]);

H5Dread(dset, H5T_IEEE_F64LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, (void*) read_buffer.data());

for (size_t i = 0; i < 10; i++) {
    size_t row_idx = i * dims[0] / 10;
    size_t col_idx = i * dims[1] / 10;
    auto element = read_buffer[row_idx * dims[1] + col_idx];
    cout << "Element at index (" << row_idx << ", " << col_idx << ") = "
         << std::setprecision(16) << element << endl;
}

H5Dclose(dset);
H5Sclose(dspace);
H5Pclose(fapl);
H5Fclose(file);

return 0;
}
