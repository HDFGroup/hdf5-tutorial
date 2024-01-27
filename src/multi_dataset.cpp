#include "hdf5.h"
#include "rest_vol_public.h"

#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

const size_t NUM_DATASETS = 2;
const size_t NUM_INTEGERS = (1000 * 1000 * 100) / NUM_DATASETS;

int main()
{
    H5rest_init();

    hid_t fapl = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_fapl_rest_vol(fapl);
    hid_t file = H5Fcreate("/home/vscode/multi_dataset.h5", H5F_ACC_TRUNC, H5P_DEFAULT, fapl);
    H5Pclose(fapl);
    
    vector<int> write_buf(NUM_DATASETS*NUM_INTEGERS);
    int* write_data[NUM_DATASETS] = {&write_buf[0], &write_buf[NUM_INTEGERS]};

    vector<int> read_buf(NUM_DATASETS*NUM_INTEGERS);
    int* read_data[NUM_DATASETS] = {&read_buf[0], &read_buf[NUM_INTEGERS]};

    hid_t dspace_ids[NUM_DATASETS], dset_ids[NUM_DATASETS], type_ids[NUM_DATASETS], sel_space_ids[NUM_DATASETS];
    
    ostringstream dset_name;
    hsize_t dims[] = { NUM_INTEGERS };

    for (size_t i = 0; i < NUM_DATASETS; ++i)
    {
        type_ids[i] = H5T_NATIVE_INT;
        dspace_ids[i] = H5Screate_simple(1, dims, NULL);
        sel_space_ids[i] = H5S_ALL;

        dset_name.str("");
        dset_name << "/dset_" << i;
        dset_ids[i] = H5Dcreate(file, dset_name.str().c_str(), type_ids[i], dspace_ids[i], H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

        for (size_t j = 0; j < NUM_INTEGERS; ++j)
            write_data[i][j] = i * 10000 + j;
    }

    H5Dwrite_multi(NUM_DATASETS, dset_ids, type_ids, sel_space_ids, sel_space_ids, H5P_DEFAULT, (const void **)write_data);

    H5Dread_multi(NUM_DATASETS, dset_ids, type_ids, sel_space_ids, sel_space_ids, H5P_DEFAULT, (void **)read_data);

    for (size_t i = 0; i < read_buf.size(); ++i)
    {
        if (read_buf[i] != write_buf[i])
        {
            cerr << "At read_buf[" << i << "], " << read_buf[i] << " != expected " << write_buf[i] << endl;
            exit(1);
        }
    }
    
    cout << "SUCCESS!" << endl;

    H5Fclose(file);
    H5rest_term();

    return 0;
}
