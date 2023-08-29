// Purpose: HDF5 tutorial for the HDF User Group 2023
// Warning: This code is not meant to be used in production!

#include <cassert>
#include <cfloat>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

#include <argparse/argparse.hpp>
#include "hdf5.h"

using namespace std;

int main(int argc, char *argv[])
{
    argparse::ArgumentParser program("hdf-tutorial");

    program.add_argument("-f", "--format")
    .help("selects output format")
    .default_value(string{"hdf5"});

    program.add_argument("-p", "--paths")
    .help("chooses the numnber of paths")
    .default_value(size_t{100})
    .scan<'u', size_t>();

    program.add_argument("-s", "--steps")
    .help("chooses the number of steps")
    .default_value(size_t{1000})
    .scan<'u', size_t>();

    program.add_argument("-d", "--dt")
    .help("chooses the time step")
    .default_value(double{0.01})
    .scan<'f', double>();

    program.add_argument("-t", "--theta")
    .help("chooses the rate of reversion to the mean")
    .default_value(double{1.0})
    .scan<'f', double>();

    program.add_argument("-m", "--mu")
    .help("chooses the long-term mean of the process")
    .default_value(double{0.0})
    .scan<'f', double>();

    program.add_argument("-g", "--sigma")
    .help("chooses the volatility of the process")
    .default_value(double{0.1})
    .scan<'f', double>();
    
    try {
        program.parse_args(argc, argv);
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        std::cerr << program;
        std::exit(1);
    }
    
    auto format = program.get<string>("--format");
    auto path_count = program.get<size_t>("--paths");
    if (path_count == 0) {
        cout << "Number of paths must be greater than zero" << endl;
        return 1;
    }
    auto step_count = program.get<size_t>("--steps");
    if (step_count == 0) {
        cout << "Number of steps must be greater than zero" << endl;
        return 1;
    }
    auto dt = program.get<double>("--dt");
    if (dt < DBL_MIN) {
        cout << "Time step must be greater than zero" << endl;
        return 1;
    }
    auto theta = program.get<double>("--theta");
    if (theta < DBL_MIN) {
        cout << "Reversion rate must be greater than zero" << endl;
        return 1;
    }
    auto mu = program.get<double>("--mu");
    auto sigma = program.get<double>("--sigma");
    if (sigma < DBL_MIN) {
        cout << "Volatility must be greater than zero" << endl;
        return 1;
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

    if (format == "txt")
    { // Write the sample paths to a text file

        ofstream file("ou_process.txt");

        file << "# paths steps dt theta mu sigma" << endl;
        file << path_count << " " << step_count << " " << dt << " " << theta << " " << mu << " " << sigma << endl;
        file << "# data" << endl;
        for (auto i = 0; i < path_count; ++i)
        {
            for (auto j = 0; j < step_count; ++j)
            {
                auto pos = i * step_count + j;
                file << ou_process[pos] << " ";
            }
            file << endl;
        }

        file.close();
    }
    else if (format == "bin")
    { // Write the sample paths to an unformatted binary file

        ofstream file("ou_process.bin", ios::out | ios::binary);
        file.write((char *)&path_count, sizeof(path_count));
        file.write((char *)&step_count, sizeof(step_count));
        file.write((char *)&dt, sizeof(dt));
        file.write((char *)&theta, sizeof(theta));
        file.write((char *)&mu, sizeof(mu));
        file.write((char *)&sigma, sizeof(sigma));
        file.write((char *)ou_process.data(), sizeof(double) * ou_process.size());
        file.close();
    }
    else if (format == "hdf5")
    { // Write the sample paths to an HDF5 file using the HDF5 C-API!
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

        auto scalar = H5Screate(H5S_SCALAR);

        { // store the time increment in a scalar (= singleton) 64-bit double attribute
            auto dt_attr = H5Acreate_by_name(file, "dataset", "dt", H5T_NATIVE_DOUBLE, scalar, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            H5Awrite(dt_attr, H5T_NATIVE_DOUBLE, &dt);
            H5Aclose(dt_attr);
        }

        { // store the reversion rate in a scalar (= singleton) 32-bit float attribute
            auto theta_attr = H5Acreate_by_name(file, "dataset", "theta", H5T_NATIVE_FLOAT, scalar, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            H5Awrite(theta_attr, H5T_NATIVE_DOUBLE, &theta); // note: we write a double, but the attribute is a float -> implicit conversion
            H5Aclose(theta_attr);
        }

        // Going crazy confuses the consumers of your data, so let's not do that. Here's what I mean

        { // store the long-term mean in a scalar UTF-8 encoded string attribute
            auto mu_str = to_string(mu);
            auto atype = H5Tcopy(H5T_C_S1);
            H5Tset_size(atype, mu_str.length());
            H5Tset_cset(atype, H5T_CSET_UTF8);
            auto mu_attr = H5Acreate_by_name(file, "dataset", "mu", atype, scalar, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            H5Awrite(mu_attr, atype, mu_str.c_str());
            H5Aclose(mu_attr);
            H5Tclose(atype);
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
            hsize_t adim[] = {1, 1, 1};
            auto atype = H5Tarray_create(H5T_NATIVE_DOUBLE, 3, adim);
            auto sigma_attr = H5Acreate_by_name(file, "dataset", "sigma1", atype, scalar, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            H5Awrite(sigma_attr, atype, &sigma); // note: we write a double, but tell the library it's a 3D array -> implicit conversion
            H5Aclose(sigma_attr);
            H5Tclose(atype);
            
        }

        H5Sclose(scalar);
        H5Fclose(file);
    }
    else
    {
        cout << "Unknown format: \"" << format << "\" : supported [hdf5, bin, txt]" << endl;
        return 1;
    }

    return 0;
}
