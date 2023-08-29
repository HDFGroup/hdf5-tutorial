// Purpose: HDF5 tutorial for the HDF User Group 2023
// Warning: This code is not meant to be used in production!
// New challenge: The length of the paths is random, and we produce them in batches

#include <cassert>
#include <cfloat>
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
    argparse::ArgumentParser program("hdf-tutorial.1");

    program.add_argument("-f", "--format")
    .help("selects output format")
    .default_value(string{"hdf5"});

    program.add_argument("-p", "--paths")
    .help("chooses the numnber of paths")
    .default_value(size_t{1000})
    .scan<'u', size_t>();

    program.add_argument("-b", "--batch")
    .help("chooses the batch size")
    .default_value(size_t{100})
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
    
    auto path_count = program.get<size_t>("--paths");
    if (path_count == 0) {
        cout << "Number of paths must be greater than zero" << endl;
        return 1;
    }
    auto batch_size = program.get<size_t>("--batch");
    if (batch_size == 0) {
        cout << "Batch size must be greater than zero" << endl;
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
         << " batch=" << batch_size
         << " dt=" << dt
         << " theta=" << theta
         << " mu=" << mu
         << " sigma=" << sigma << endl;

    auto file = H5Fcreate("ou_process.1.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    // create the paths dataset
    hsize_t dimsf[] = {0, H5S_UNLIMITED};  // we cannot know the size of the paths dataset in advance
    auto space = H5Screate_simple(1, dimsf, &dimsf[1]);
    auto lcpl = H5Pcreate(H5P_LINK_CREATE);
    H5Pset_create_intermediate_group(lcpl, 1);
    auto dcpl = H5Pcreate(H5P_DATASET_CREATE);
    hsize_t cdims[] = {128 * 1024};
    H5Pset_chunk(dcpl, 1, cdims);
    auto paths = H5Dcreate(file, "/paths/data", H5T_NATIVE_DOUBLE, space, lcpl, dcpl, H5P_DEFAULT);
    H5Pclose(dcpl);
    H5Sclose(space);

    // create the descriptors (= offset into paths dataset)
    dimsf[0] = path_count;  // we know the size of the descriptors dataset in advance
    space = H5Screate_simple(1, dimsf, NULL);
    auto descr = H5Dcreate(file, "/paths/descr", H5T_NATIVE_HSIZE, space, lcpl, H5P_DEFAULT, H5P_DEFAULT);
    H5Sclose(space);
    H5Pclose(lcpl);

    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<unsigned int> path_len_dist(1, USHRT_MAX);  // path length is between 1 and 65535
    normal_distribution<double> dist(0.0, sqrt(dt));  // N(0, dt)

    // vectors to store the paths and the descriptors in a batch
    vector<double> ou_process(0);
    vector<hsize_t> offset(1, 0);

    for (auto p = 0; p < path_count; p += batch_size)
    {
        if (p + batch_size > path_count)  // last batch
        {
            batch_size = path_count - p;
        }
        cout << "Generating paths " << p << " to " << p + batch_size << endl;

        size_t pos = 0;  // offset into the ou_process vector

        // Generate a batch of paths and offsets
        for (auto i = 0; i < batch_size; ++i)
        {
            
            size_t step_count = path_len_dist(generator);  // Generate random path length
            ou_process.resize(pos + step_count);  // Resize the vector to make room for the new path
            // Generate the path
            ou_process[pos] = 0; // start at x = 0
            for (auto j = 1; j < step_count; ++j)
            {
                auto dW = dist(generator);
                ++pos;  // advance the offset
                ou_process[pos] = ou_process[pos - 1] + theta * (mu - ou_process[pos - 1]) * dt + sigma * dW;
            }

            offset.push_back(offset.back() + (hsize_t)step_count - 1);  // -1 because we want the starting point of the next path
        }

        // write the batch to the HDF5 file

        { // write the paths and path descriptors
            auto path_space = H5Dget_space(paths);  // pretend we didn't count along the way
            hsize_t path_elem_count = (hsize_t) H5Sget_simple_extent_npoints(path_space);  // 1D dataset
            H5Sclose(path_space);
            hsize_t path_dims[] = {path_elem_count + (hsize_t) ou_process.size()};
            H5Dset_extent(paths, path_dims);   // resize the dataset/extend the dataspace

            path_space = H5Dget_space(paths);  // get the new dataspace to make the correct selection
            hsize_t start[] = {path_elem_count};
            hsize_t count[] = {(hsize_t) ou_process.size()};
            H5Sselect_hyperslab(path_space, H5S_SELECT_SET, start, NULL, count, NULL);
            auto mem_space = H5Screate_simple(1, count, NULL);
            H5Sselect_all(mem_space);  // we want to write the whole vector
            
            H5Dwrite(paths, H5T_NATIVE_DOUBLE, mem_space, path_space, H5P_DEFAULT, ou_process.data());
            H5Sclose(mem_space);
            H5Sclose(path_space);

            // same pattern for the descriptors, except that we don't need to resize the dataset
            auto descr_space = H5Dget_space(descr);
            start[0] = (hsize_t) p;
            count[0] = (hsize_t) offset.size()-1;
            H5Sselect_hyperslab(descr_space, H5S_SELECT_SET, start, NULL, count, NULL);
            mem_space = H5Screate_simple(1, count, NULL);
            H5Sselect_all(mem_space);

            H5Dwrite(descr, H5T_NATIVE_HSIZE, mem_space, descr_space, H5P_DEFAULT, offset.data());
            H5Sclose(mem_space);
            H5Sclose(descr_space);
        }

        // reset the vectors and offset
        ou_process.clear();
        pos = 0;
        offset[0] = offset.back();  // the first offset is the last offset of the previous batch
        offset.resize(1);
    }

    { // to make the file self-describing, we add some metadata in the form of attributes
        auto scalar = H5Screate(H5S_SCALAR);
        auto dt_attr = H5Acreate_by_name(file, "paths", "dt", H5T_NATIVE_DOUBLE, scalar, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        H5Awrite(dt_attr, H5T_NATIVE_DOUBLE, &dt);
        H5Aclose(dt_attr);
        auto theta_attr = H5Acreate_by_name(file, "paths", "theta", H5T_NATIVE_DOUBLE, scalar, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        H5Awrite(theta_attr, H5T_NATIVE_DOUBLE, &theta);
        H5Aclose(theta_attr);
        auto mu_attr = H5Acreate_by_name(file, "paths", "mu", H5T_NATIVE_DOUBLE, scalar, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        H5Awrite(mu_attr, H5T_NATIVE_DOUBLE, &mu);
        H5Aclose(mu_attr);
        auto sigma_attr = H5Acreate_by_name(file, "paths", "sigma", H5T_NATIVE_DOUBLE, scalar, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        H5Awrite(sigma_attr, H5T_NATIVE_DOUBLE, &sigma);
        H5Aclose(sigma_attr);
        H5Sclose(scalar);
    }

    H5Dclose(descr);
    H5Dclose(paths);
    H5Fclose(file);

    return 0;
}
