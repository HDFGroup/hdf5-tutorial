#include "parse_arguments.hpp"
#include "ou_sampler.hpp"

#include "hdf5.h"
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
    size_t path_count, step_count;
    double dt, theta, mu, sigma;

    argparse::ArgumentParser program("ou_hdf5");
    set_options(program);
    program.parse_args(argc, argv);
    get_arguments(program, path_count, step_count, dt, theta, mu, sigma);

    cout << "Running with parameters:"
         << " paths=" << path_count << " steps=" << step_count
         << " dt=" << dt << " theta=" << theta << " mu=" << mu << " sigma=" << sigma << endl;

    vector<double> ou_process;
    ou_sampler(ou_process, path_count, step_count, dt, theta, mu, sigma);
    
    //
    // Write the sample paths to an HDF5 file using the HDF5 C-API!
    //

    auto file = H5Fcreate("ou_process.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    { // create & write the dataset
        hsize_t dimsf[] = {(hsize_t)path_count, (hsize_t)step_count};
        auto space = H5Screate_simple(2, dimsf, NULL);
        auto dataset = H5Dcreate(file, "/dataset", H5T_NATIVE_DOUBLE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, ou_process.data());
        H5Dclose(dataset);
        H5Sclose(space);
    }

    { // make the file self-describing by adding a few attributes to `dataset`
        auto scalar = H5Screate(H5S_SCALAR);
        auto acpl = H5Pcreate(H5P_ATTRIBUTE_CREATE);
        H5Pset_char_encoding(acpl, H5T_CSET_UTF8);
        
        auto set_attribute = [&](const string& name, const double& value) {
            auto attr = H5Acreate_by_name(file, "dataset", name.c_str(), H5T_NATIVE_DOUBLE, scalar, acpl, H5P_DEFAULT, H5P_DEFAULT);
            H5Awrite(attr, H5T_NATIVE_DOUBLE, &value);
            H5Aclose(attr);
        };
        set_attribute("dt", dt);
        set_attribute("θ", theta);
        set_attribute("μ", mu);
        set_attribute("σ", sigma);

        H5Pclose(acpl);
        H5Sclose(scalar);
    }

    H5Fclose(file);

    return 0;
}
