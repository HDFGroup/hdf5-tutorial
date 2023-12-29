#include "parse_arguments1.hpp"
#include "ou_sampler1.hpp"

#include "hdf5.h"
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
    size_t path_count, batch_size;
    double dt, theta, mu, sigma;

    argparse::ArgumentParser program("ou_hdf5.1");
    set_options1(program);
    program.parse_args(argc, argv);
    get_arguments1(program, path_count, batch_size, dt, theta, mu, sigma);

    cout << "Running with parameters:"
         << " paths=" << path_count << " batch=" << batch_size
         << " dt=" << dt << " theta=" << theta << " mu=" << mu << " sigma=" << sigma << endl;

    auto file = H5Fcreate("ou_process.1.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    hid_t paths, descr;

    auto lcpl = H5Pcreate(H5P_LINK_CREATE);
    H5Pset_create_intermediate_group(lcpl, 1);

    { // create the extendible `paths/data` dataset
        hsize_t dimsf[] = {0, H5S_UNLIMITED};
        auto space = H5Screate_simple(1, dimsf, &dimsf[1]);
        auto dcpl = H5Pcreate(H5P_DATASET_CREATE);
        hsize_t cdims[] = {128 * 1024};
        H5Pset_chunk(dcpl, 1, cdims);
        paths = H5Dcreate(file, "/paths/data", H5T_NATIVE_DOUBLE, space, lcpl, dcpl, H5P_DEFAULT);
        H5Pclose(dcpl);
        H5Sclose(space);
    }
    { // create the fixed size descriptors (= offsets into paths dataset) dataset `paths/descr`
        hsize_t dimsf[] = { (hsize_t) path_count };
        auto space = H5Screate_simple(1, dimsf, NULL);
        descr = H5Dcreate(file, "/paths/descr", H5T_NATIVE_HSIZE, space, lcpl, H5P_DEFAULT, H5P_DEFAULT);
        H5Sclose(space);    
    }
    
    H5Pclose(lcpl);

    // vectors to store the paths and the descriptors in a batch
    vector<double> ou_process;
    vector<hsize_t> offset;

    // track the global (=across batches) offset
    hsize_t global_pos = 0;  // 

    for (size_t p = 0; p < path_count; p += batch_size)
    {
        if (p + batch_size > path_count)  // last batch
            batch_size = path_count - p;
        cout << "Generating paths " << p << " to " << p + batch_size << endl;
        
        // Generate a batch of paths and offsets
        ou_sampler1(ou_process, offset, batch_size, dt, theta, mu, sigma);

        { // write the paths
            auto path_space = H5Dget_space(paths);
            hsize_t path_elem_count = (hsize_t) H5Sget_simple_extent_npoints(path_space);  // 1D dataset
            H5Sclose(path_space);
            hsize_t path_dims[] = {path_elem_count + (hsize_t) ou_process.size()};

            // make room for more data
            H5Dset_extent(paths, path_dims);   
            // get the updated dataspace to make the correct selection(!)
            path_space = H5Dget_space(paths);

            hsize_t start[] = {path_elem_count};
            hsize_t count[] = {(hsize_t) ou_process.size()};
            H5Sselect_hyperslab(path_space, H5S_SELECT_SET, start, NULL, count, NULL);
            auto mem_space = H5Screate_simple(1, count, NULL);
            H5Sselect_all(mem_space);  // we want to write the whole vector
            H5Dwrite(paths, H5T_NATIVE_DOUBLE, mem_space, path_space, H5P_DEFAULT, ou_process.data());
            H5Sclose(mem_space);
            H5Sclose(path_space);
        }
        { // write the path descriptors
            auto descr_space = H5Dget_space(descr);
            hsize_t start[] = { (hsize_t) p };
            hsize_t count[] = { (hsize_t) offset.size()-1};  // offset has one extra element
            H5Sselect_hyperslab(descr_space, H5S_SELECT_SET, start, NULL, count, NULL);
            auto mem_space = H5Screate_simple(1, count, NULL);
            H5Sselect_all(mem_space);
            // the offsets are 0-based and we must correct this for the global offset
            std::for_each(offset.begin(), offset.end(), [&](hsize_t &n){ n+=global_pos; });
            global_pos = offset.back();
            H5Dwrite(descr, H5T_NATIVE_HSIZE, mem_space, descr_space, H5P_DEFAULT, offset.data());
            H5Sclose(mem_space);
            H5Sclose(descr_space);
        }
    }
    
    { // make the file self-describing by adding a few attributes to `paths`
        auto scalar = H5Screate(H5S_SCALAR);
        auto acpl = H5Pcreate(H5P_ATTRIBUTE_CREATE);
        H5Pset_char_encoding(acpl, H5T_CSET_UTF8);
        auto set_attribute = [&](const string& name, const double& value) {
            auto attr = H5Acreate_by_name(file, "paths", name.c_str(), H5T_NATIVE_DOUBLE, scalar, acpl, H5P_DEFAULT, H5P_DEFAULT);
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

    H5Dclose(descr);
    H5Dclose(paths);
    H5Fclose(file);

    return 0;
}
