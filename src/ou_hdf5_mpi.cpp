#include "parse_arguments.hpp"
#include "parse_arguments2.hpp"
#include "partitioner.hpp"
#include "ou_sampler.hpp"

#include "hdf5.h"
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
    // <ALTERNATIVE>:: The standard MPI IO File Driver only requires:
    //
    // MPI_Init(&argc, &argv);
    //
    // However, the subfiling VFD requires MPI_Init_thread with MPI_THREAD_MULTIPLE

    int mpi_thread_required = MPI_THREAD_MULTIPLE;
    int mpi_thread_provided = 0;

    MPI_Init_thread(&argc, &argv, mpi_thread_required, &mpi_thread_provided);
    if (mpi_thread_provided < mpi_thread_required)
    {
      cout << "MPI_THREAD_MULTIPLE not supported" << endl;
      MPI_Abort(MPI_COMM_WORLD, -1);
    }

    // Get the number of processes
    int nprocs;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    // Get the rank of the process
    int myid;
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    size_t path_count, step_count;
    double dt, theta, mu, sigma;
#ifdef H5_HAVE_SUBFILING_VFD    
    bool subfiling;
#endif    

    argparse::ArgumentParser program("ou_hdf5_mpi");
    set_options(program);
    program.parse_args(argc, argv);
#ifdef H5_HAVE_SUBFILING_VFD
    get_arguments2(program, path_count, step_count, dt, theta, mu, sigma, subfiling);
#else
    get_arguments(program, path_count, step_count, dt, theta, mu, sigma);
#endif     

    if (myid == 0)
    {
        cout << "Running on " << nprocs << " MPI ranks with parameters:"
            << " paths=" << path_count << " steps=" << step_count
            << " dt=" << dt << " theta=" << theta << " mu=" << mu << " sigma=" << sigma
#ifdef H5_HAVE_SUBFILING_VFD
            << " subfiling=" << subfiling
#endif
            << endl;
    }

    vector<double> ou_process;
    
    size_t start, stop;
    partition_work(path_count, myid, nprocs, start, stop);
    size_t my_path_count = stop - start + 1;

    ou_sampler(ou_process, my_path_count, step_count, dt, theta, mu, sigma);
    
    // Use the Subfiling or MPI-IO driver
    hid_t fapl = H5Pcreate(H5P_FILE_ACCESS);
#ifdef H5_HAVE_SUBFILING_VFD
    if(subfiling)
      H5Pset_fapl_subfiling(fapl, NULL);
    else
#endif
      H5Pset_fapl_mpio(fapl, MPI_COMM_WORLD, MPI_INFO_NULL);

    //
    // Write the sample paths to an HDF5 file using the HDF5 C-API!
    //
    auto file = H5Fcreate("ou_process.2.h5", H5F_ACC_TRUNC, H5P_DEFAULT, fapl);

    { // create & write the dataset
        hsize_t dimsf[] = {(hsize_t)path_count, (hsize_t)step_count};
        auto filespace = H5Screate_simple(2, dimsf, NULL);
        auto dataset = H5Dcreate(file, "/dataset", H5T_NATIVE_DOUBLE, filespace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        
        // Define, by rank, a selection in memory and write it to a hyperslab in the file.
        hsize_t count[]  = {my_path_count, step_count};
        hsize_t offset[] = {start, 0};
        
        hid_t memspace = H5Screate_simple(2, count, NULL);

        // Select hyperslab in the file.
        // hid_t filespace = H5Dget_space(dataset);
        H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offset, NULL, count, NULL);

        // <OPTIONAL> Create property list for collective dataset write.
        hid_t dxpl = H5Pcreate(H5P_DATASET_XFER);
        H5Pset_dxpl_mpio(dxpl, H5FD_MPIO_COLLECTIVE);

        H5Dwrite(dataset, H5T_NATIVE_DOUBLE, memspace, filespace, dxpl, ou_process.data());

        // housekeeping
        H5Pclose(dxpl);
        H5Sclose(memspace);
        H5Dclose(dataset);
        H5Pclose(fapl);
        H5Sclose(filespace);
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

    MPI_Finalize();

    return 0;
}
