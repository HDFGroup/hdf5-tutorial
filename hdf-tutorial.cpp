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
    hid_t dt_attr_space, theta_attr_space, mu_attr_space, sigma_attr_space; /* attribute dataspace handles */
    hid_t dt_attr, theta_attr, mu_attr, sigma_attr; /* attribute handles */
    hid_t atype;      /* string type*/
    hsize_t dimsf[2];            /* dataset dimensions */
    hsize_t adim[] = {1, 1, 1};  /* dimensions for attribute matrix */

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

    /* Creat attribute for dt, theta, mu, sigma using differing types. */
    dt_attr_space = H5Screate(H5S_SCALAR);
    dt_attr = H5Acreate2(file, "dt", H5T_NATIVE_DOUBLE, dt_attr_space, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(dt_attr, H5T_NATIVE_DOUBLE, &dt);

    float f_theta = (float) theta;
    theta_attr_space = H5Screate(H5S_SCALAR);
    theta_attr = H5Acreate2(file, "theta", H5T_NATIVE_FLOAT, theta_attr_space, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(theta_attr, H5T_NATIVE_FLOAT, &f_theta);

    std::string mu_str = std::to_string(mu);
    mu_attr_space = H5Screate(H5S_SCALAR);
    atype = H5Tcopy(H5T_C_S1);
    H5Tset_size(atype, mu_str.length());
    H5Tset_cset(datatype_id, H5T_CSET_UTF8);
    mu_attr = H5Acreate2(file, "mu" atype, mu_attr_space, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(mu_attr, atype, mu_str.c_str());

    double mu_mat[1][1][1];
    mu_mat[0][0][0] = sigma;
    sigma_attr_space = H5Screate(H5S_SIMPLE);
    H5Sset_extent_simple(sigma_attr_space, 3, adim, NULL);
    sigma_attr = H5Acreate2(file, "sigma" H5T_NATIVE_DOUBLE, sigma_attr_space, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(sigma_attr, H5T_NATIVE_DOUBLE, mu_mat);

    /* Close attributes. */
    H5Aclose(dt_attr);
    H5Aclose(theta_attr);
    H5Aclose(mu_attr);
    H5Aclose(sigma_attr);

    /* Close attribute and file dataspaces. */
    H5Sclose(dt_attr_space);
    H5Sclose(theta_attr_space);
    H5Sclose(mu_attr_space);
    H5Sclose(sigma_attr_space);
    H5Tclose(atype);

    /* Close the dataset. */
    H5Dclose(dataset);

    /* Close the file. */
    H5Fclose(file);

    return 0;
}
