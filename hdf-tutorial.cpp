#include <iostream>
#include <random>
#include <vector>
#include <map>
#include <hdf5.h>

using namespace std;

int main(int argc, char *argv[]) {
    int paths = 100;       /* Number of paths */
    int steps = 1000;      /* Number of steps */
    double dt = 0.01;      /* Time increment */
    double theta = 1.0;    /* Rate of reversion to the mean */
    double mu = 0.0;       /* Long-term mean of the process */
    double sigma = 0.1;    /* Volatility of the process */

    map<string, string> args; /* Create a map to store user inputs */

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        size_t pos = arg.find('=');
        if (pos == string::npos) {
            continue;
        }
        string key = arg.substr(0, pos);
        string value = arg.substr(pos + 1);
        args[key] = value;
    }

    /* Check if any of the user's inputs need to be used against map. */
    if (args.count("paths")) {
        paths = stoi(args["paths"]);
    }

    if (args.count("steps")) {
        steps = stoi(args["steps"]);
    }

    if (args.count("dt")) {
        dt = stod(args["dt"]);
    }

    if (args.count("theta")) {
        theta = stod(args["theta"]);
    }
    
    if (args.count("mu")) {
        mu = stod(args["mu"]);
    }

    if (args.count("sigma")) {
        sigma = stod(args["sigma"]);
    }
 
    /* Populate an array according to the Ornstein-Uhlenbeck process */
    random_device rd;
    mt19937 generator(rd());
    normal_distribution<double> dist(0.0, sqrt(dt));
    double* ou_process = new double[paths * steps];
    
    for(int i = 0; i < paths; ++i) {
        *(ou_process + i * steps) = 0;   /* Start at x = 0 */
        for(int j = 1; j < steps; ++j)
        {
            double dW = dist(generator); 
            *(ou_process + i * steps + j) = *(ou_process + i * steps + j - 1) + theta * (mu - *(ou_process + i * steps + j - 1)) * dt + sigma * dW;
        }
    }

    /* Write the paths to an HDF5 file using the HDF5 C API */
    hid_t file, dataset, space;  /* Handles */
    hid_t dt_attr_space, theta_attr_space, mu_attr_space, sigma_attr_space; /* Attribute dataspace handles */
    hid_t dt_attr, theta_attr, mu_attr, sigma_attr; /* Attribute handles */
    hid_t atype;      /* String type */
    hsize_t dimsf[2];            /* Dataset dimensions */
    hsize_t adim[] = {1, 1, 1};  /* Dimensions for attribute matrix */
    
    double mu_mat[1][1][1];     /* Declaration of matrix attribute */
    string mu_str;    /* Declaration of string attribute */

    /* Initialize the dimension array */
    dimsf[0] = paths;
    dimsf[1] = steps;

    /* Create a new file using default properties */
    file = H5Fcreate("ou_process.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /* Create the dataspace for the dataset */
    space = H5Screate_simple(2, dimsf, NULL);

    /* Create the dataset with default properties and close dataspace */
    dataset = H5Dcreate(file, "/dataset", H5T_NATIVE_DOUBLE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    H5Sclose(space);

    /* Write the dataset */
    H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, ou_process.data());

    /* Attributes can be any type, whether it be user defined, native, or even an array */
    /* Create attribute for dt, theta, mu, sigma using different types of attributes */
    dt_attr_space = H5Screate(H5S_SCALAR); /* Create the dataspace for the attribute*/
    dt_attr = H5Acreate2(file, "dt", H5T_NATIVE_DOUBLE, dt_attr_space, H5P_DEFAULT, H5P_DEFAULT); /* Create a double attribute */
    H5Awrite(dt_attr, H5T_NATIVE_DOUBLE, &dt); /* Write the double attribute */

    float f_theta = (float) theta; /* Convert theta to float */
    theta_attr_space = H5Screate(H5S_SCALAR); /* Create the dataspace for the attribute*/
    theta_attr = H5Acreate2(file, "theta", H5T_NATIVE_FLOAT, theta_attr_space, H5P_DEFAULT, H5P_DEFAULT); /* Create a float attribute */
    H5Awrite(theta_attr, H5T_NATIVE_FLOAT, &f_theta); /* Write the float attribute */

    mu_str = to_string(mu); /* Convert mu to a string */
    mu_attr_space = H5Screate(H5S_SCALAR); /* Create the dataspace for the attribute*/
    atype = H5Tcopy(H5T_C_S1); /* Create the datatype for this attribute */
    H5Tset_size(atype, mu_str.length()); /* Set size */
    H5Tset_cset(atype, H5T_CSET_UTF8); /* Set to UTF-8 */
    mu_attr = H5Acreate2(file, "mu", atype, mu_attr_space, H5P_DEFAULT, H5P_DEFAULT); /* Create a string attribute */
    H5Awrite(mu_attr, atype, mu_str.c_str()); /* Write the string attribute */

    mu_mat[0][0][0] = sigma; /* Write sigma to the example matrix */
    sigma_attr_space = H5Screate(H5S_SIMPLE); /* Create the dataspace for the attribute*/
    H5Sset_extent_simple(sigma_attr_space, 3, adim, NULL); /* Set dataspace dimensions */
    sigma_attr = H5Acreate2(file, "sigma", H5T_NATIVE_DOUBLE, sigma_attr_space, H5P_DEFAULT, H5P_DEFAULT); /* Create matrix attribute */
    H5Awrite(sigma_attr, H5T_NATIVE_DOUBLE, mu_mat); /* Write the matrix attribute */

    /* Close attributes */
    H5Aclose(dt_attr);
    H5Aclose(theta_attr);
    H5Aclose(mu_attr);
    H5Aclose(sigma_attr);

    /* Close attribute and file dataspaces */
    H5Sclose(dt_attr_space);
    H5Sclose(theta_attr_space);
    H5Sclose(mu_attr_space);
    H5Sclose(sigma_attr_space);
    H5Tclose(atype);

    /* Close the dataset */
    H5Dclose(dataset);

    /* Close the file */
    H5Fclose(file);

    return 0;
}
