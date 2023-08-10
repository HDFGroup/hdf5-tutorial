// Purpose: HDF5 tutorial for the HDF User Group 2023
// Warning: This code is not meant to be used in production!

#include <cassert>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

#include "hdf5.h"

using namespace std;

int main(int argc, char *argv[])
{
    // Create a map to store user inputs using the format key=value
    map<string, string> args;

    for (auto i = 1; i < argc; i++)
    {
        string arg = argv[i];
        size_t pos = arg.find('=');
        if (pos == string::npos)
        {
            continue;
        }
        string key = arg.substr(0, pos);
        string value = arg.substr(pos + 1);
        args[key] = value;
    }

    // Parameter defaults
    size_t path_count = 100;
    size_t step_count = 1000;
    double dt = 0.01;   // time step
    double theta = 1.0; // rate of reversion to the mean
    double mu = 0.0;    // long-term mean of the process
    double sigma = 0.1; // volatility of the process

    { // Parse user inputs
        if (args.count("paths"))
        {
            path_count = stoul(args["paths"]);
            assert(path_count > 0);
        }
        if (args.count("steps"))
        {
            step_count = stoul(args["steps"]);
            assert(step_count > 0);
        }
        if (args.count("dt"))
        {
            dt = stod(args["dt"]);
            assert(dt > 0);
        }
        if (args.count("theta"))
        {
            theta = stod(args["theta"]);
            assert(theta > 0);
        }
        if (args.count("mu"))
        {
            mu = stod(args["mu"]);
        }
        if (args.count("sigma"))
        {
            sigma = stod(args["sigma"]);
            assert(sigma > 0);
        }
    }

    cout << "Running with parameters:"
         << " paths=" << path_count
         << " steps=" << step_count
         << " dt=" << dt
         << " theta=" << theta
         << " mu=" << mu
         << " sigma=" << sigma << endl;

    vector<double> ou_process(path_count * step_count);

    { // Populate an array according to the Ornstein-Uhlenbeck process
        random_device rd;
        mt19937 generator(rd());
        normal_distribution<double> dist(0.0, sqrt(dt));

        for (auto i = 0; i < path_count; ++i)
        {
            ou_process[i * step_count] = 0; /* Start at x = 0 */
            for (auto j = 1; j < step_count; ++j)
            {
                auto dW = dist(generator);
                auto pos = i * step_count + j;
                ou_process[pos] = ou_process[pos - 1] + theta * (mu - ou_process[pos - 1]) * dt + sigma * dW;
            }
        }
    }

    // Write the sample paths to an HDF5 file using the HDF5 C-API!
    // There are fine HDF5 C++ APIs available, but this is not today's topic.

    // Create a new file using default properties
    auto file = H5Fcreate("ou_process.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    { // create & write the dataset
        hsize_t dimsf[] = {(hsize_t)path_count, (hsize_t)step_count};
        auto space = H5Screate_simple(2, dimsf, NULL);
        auto dataset = H5Dcreate(file, "/dataset", H5T_NATIVE_DOUBLE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, ou_process.data());
        H5Dclose(dataset);
        H5Sclose(space);
    }

    // To make the file self-describing, we add some metadata in the form of attributes

    { // store the time increment in a scalar (= singleton) 64-bit double attribute
        auto dt_attr_space = H5Screate(H5S_SCALAR);
        auto dt_attr = H5Acreate_by_name(file, "dataset", "dt", H5T_NATIVE_DOUBLE, dt_attr_space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        H5Awrite(dt_attr, H5T_NATIVE_DOUBLE, &dt);
        H5Aclose(dt_attr);
        H5Sclose(dt_attr_space);
    }

    { // store the reversion rate in a scalar (= singleton) 32-bit float attribute
        auto theta_attr_space = H5Screate(H5S_SCALAR);
        auto theta_attr = H5Acreate_by_name(file, "dataset", "theta", H5T_NATIVE_FLOAT, theta_attr_space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        H5Awrite(theta_attr, H5T_NATIVE_DOUBLE, &theta); // note: we write a double, but the attribute is a float -> implicit conversion
        H5Aclose(theta_attr);
        H5Sclose(theta_attr_space);
    }

    // Going crazy confuses the consumers of your data, so let's not do that. Here's what I mean

    { // store the long-term mean in a scalar UTF-8 encoded string attribute
        auto mu_str = to_string(mu);
        auto mu_attr_space = H5Screate(H5S_SCALAR);
        auto atype = H5Tcopy(H5T_C_S1);
        H5Tset_size(atype, mu_str.length());
        H5Tset_cset(atype, H5T_CSET_UTF8);
        auto mu_attr = H5Acreate_by_name(file, "dataset", "mu", atype, mu_attr_space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        H5Awrite(mu_attr, atype, mu_str.c_str());
        H5Aclose(mu_attr);
        H5Tclose(atype);
        H5Sclose(mu_attr_space);
    }

    { // go crazy and store the volatility in a 3D array attribute
        auto sigma_attr_space = H5Screate(H5S_SIMPLE);
        hsize_t adim[] = {1, 1, 1};
        H5Sset_extent_simple(sigma_attr_space, 3, adim, NULL);
        auto sigma_attr = H5Acreate_by_name(file, "dataset", "sigma", H5T_NATIVE_DOUBLE, sigma_attr_space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        H5Awrite(sigma_attr, H5T_NATIVE_DOUBLE, &sigma);
        H5Aclose(sigma_attr);
        H5Sclose(sigma_attr_space);
    }

    { // here's another crazy way to store the volatility in a scalar(!) 3D array attribute
        auto sigma_attr_space = H5Screate(H5S_SCALAR);
        hsize_t adim[] = {1, 1, 1};
        auto atype = H5Tarray_create(H5T_NATIVE_DOUBLE, 3, adim);
        auto sigma_attr = H5Acreate_by_name(file, "dataset", "sigma1", atype, sigma_attr_space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        auto sigma_ptr = &sigma;
        H5Awrite(sigma_attr, atype, &sigma); // note: we write a double, but tell the library it's a 3D array -> implicit conversion
        H5Aclose(sigma_attr);
        H5Tclose(atype);
        H5Sclose(sigma_attr_space);
    }
    
    H5Fclose(file);

    return 0;
}
