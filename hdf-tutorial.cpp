#include <iostream>
#include <random>
#include <vector>
#include <hdf5.h>

using namespace std;

int main() {
    int paths = 100;       // number of paths
    int steps = 1000;      // number of steps
    double dt = 0.01;      // time increment
    double theta = 1.0;    // rate of reversion to the mean
    double mu = 0.0;       // long-term mean of the process
    double sigma = 0.1;    // volatility of the process

    std::random_device rd;
    std::mt19937 generator(rd());
    std::normal_distribution<double> dist(0.0, std::sqrt(dt));
    
    std::vector<std::vector<double>> ou_process(paths, std::vector<double>(steps));
    
    for(int i = 0; i < paths; ++i)
    {
        ou_process[i][0] = 0.0;   // start at X=0
        for(int j = 1; j < steps; ++j)
        {
            double dW = dist(generator); 
            ou_process[i][j] = ou_process[i][j-1] + theta * (mu - ou_process[i][j-1]) * dt + sigma * dW;
        }
    }

    // Write the paths to an HDF5 file using HDF5 C-API
    hid_t file, dataset, space;  /* handles */
    hsize_t dimsf[2];            /* dataset dimensions */

    /* Initialize the dimension array. */
    dimsf[0] = paths;
    dimsf[1] = steps;

    /* Create a new file using default properties. */
    file = H5Fcreate("ou_process.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /* Create the dataspace for the dataset. */
    space = H5Screate_simple(2, dimsf, NULL);

    /* Create the dataset with default properties and close dataspace. */
    dataset = H5Dcreate(file, "/dataset", H5T_NATIVE_DOUBLE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    H5Sclose(space);

    /* Write the dataset. */
    H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, ou_process.data());

    /* Close the dataset. */
    H5Dclose(dataset);

    /* Close the file. */
    H5Fclose(file);

    return 0;
}
