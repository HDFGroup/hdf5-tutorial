#include "docstring.hpp"
#include "ou_sampler.hpp"
#include "rest_vol_public.h"
#include "hdf5.h"
#include <iostream>
#include <vector>

using namespace std;

int main()
{
    const size_t path_count = 100, step_count = 1000;
    const double dt = 0.01, theta = 1.0, mu = 0.0, sigma = 0.1;

    cout << "Running with parameters:"
         << " paths=" << path_count << " steps=" << step_count
         << " dt=" << dt << " theta=" << theta << " mu=" << mu << " sigma=" << sigma << endl;

    vector<double> ou_process;
    ou_sampler(ou_process, path_count, step_count, dt, theta, mu, sigma);
    
    //
    // Write the sample paths to an HDF5 file using the HDF5 REST VOL!
    //

    H5rest_init();
    auto fapl = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_fapl_rest_vol(fapl);

    auto file = H5Fcreate("/home/vscode/ou_restvol.h5", H5F_ACC_TRUNC, H5P_DEFAULT, fapl);

    add_docstring(file, ".", "source", "https://github.com/HDFGroup/hdf5-tutorial");

    { // create & write the dataset
        hsize_t dimsf[] = {(hsize_t)path_count, (hsize_t)step_count};
        auto space = H5Screate_simple(2, dimsf, NULL);
        auto dataset = H5Dcreate(file, "/dataset", H5T_NATIVE_DOUBLE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, ou_process.data());
        H5Dclose(dataset);
        H5Sclose(space);
    }

    { // make the file self-describing by adding a few attributes to `dataset`
        add_docstring(file, "dataset", "comment", "This dataset contains sample paths of an Ornstein-Uhlenbeck process.");
        add_docstring(file, "dataset", "Wikipedia", "https://en.wikipedia.org/wiki/Ornstein%E2%80%93Uhlenbeck_process");
        add_docstring(file, "dataset", "rows", "path");
        add_docstring(file, "dataset", "columns", "time");
        
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

    H5Pclose(fapl);
    H5Fclose(file);

    H5rest_term();

    return 0;
}
