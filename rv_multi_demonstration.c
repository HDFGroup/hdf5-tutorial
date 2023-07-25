#include "hdf5.h"
#include "rest_vol_public.h"
#include <stdlib.h>

#define HSDS_FILENAME_PREFIX "/home/test_user1/"

#define FILENAME_BUFFER_SIZE 1024
#define NUM_DATASETS 2
#define NUM_INTEGERS (1000 * 1000 * 100)/NUM_DATASETS

#define ERROR(msg)                \
    fprintf(stderr, "%s\n", msg); \
    exit(1); \

#define LOG(msg) \
    fprintf(stderr, "%s\n", msg);

/* For displaying log messages after the progress bars for dataset read/write */
#define SCROLL_AMNT (NUM_DATASETS < 21 ? NUM_DATASETS : 21)

int main(void)
{

    hid_t file_id = H5I_INVALID_HID;
    hid_t fapl_id = H5I_INVALID_HID;
    hid_t fcpl_id = H5I_INVALID_HID;
    hid_t dcpl_id = H5I_INVALID_HID;
    hid_t dapl_id = H5I_INVALID_HID;
    hid_t dxpl_id = H5I_INVALID_HID;
    hid_t dset_ids[NUM_DATASETS];
    hid_t dspace_ids[NUM_DATASETS];
    hid_t type_ids[NUM_DATASETS];
    hid_t sel_space_ids[NUM_DATASETS];

    hsize_t dims[1] = {NUM_INTEGERS};

    int *write_data[NUM_DATASETS];
    int *read_data[NUM_DATASETS];

    LOG("Initializing REST VOL...")
    H5rest_init();

    /* Set property lists to use REST VOL */
    fcpl_id = H5Pcreate(H5P_FILE_CREATE);
    fapl_id = H5Pcreate(H5P_FILE_ACCESS);
    dcpl_id = H5Pcreate(H5P_DATASET_CREATE);
    dapl_id = H5Pcreate(H5P_DATASET_ACCESS);
    dxpl_id = H5Pcreate(H5P_DATASET_XFER);

    H5Pset_fapl_rest_vol(fapl_id);

    LOG("Creating file...")

    if ((file_id = H5Fcreate(HSDS_FILENAME_PREFIX "my_file.h5", H5F_ACC_TRUNC, fcpl_id, fapl_id)) < 0)
    {
        ERROR("Failed to create file");
    }

    LOG("Setting up data...");
    fprintf(stderr, "  %d Datasets of %d integers each\n", NUM_DATASETS, NUM_INTEGERS);

    for (size_t i = 0; i < NUM_DATASETS; i++)
    {
        dspace_ids[i] = H5Screate_simple(1, dims, NULL);

        char dset_name[FILENAME_BUFFER_SIZE];

        snprintf(dset_name, FILENAME_BUFFER_SIZE, "%s%zu", "dset", i);

        if ((dset_ids[i] = H5Dcreate(file_id, dset_name, H5T_NATIVE_INT, dspace_ids[i], H5P_DEFAULT, dcpl_id, dapl_id)) < 0)
        {
            ERROR("Failed to create dataset");
        }

        type_ids[i] = H5T_NATIVE_INT;
        sel_space_ids[i] = H5S_ALL;

        if ((write_data[i] = calloc(NUM_INTEGERS, sizeof(int))) == NULL)
        {
            ERROR("Failed to allocate memory");
        }

        if ((read_data[i] = calloc(NUM_INTEGERS, sizeof(int))) == NULL)
        {
            ERROR("Failed to allocate memory");
        }

        for (size_t j = 0; j < NUM_INTEGERS; j++)
        {
            write_data[i][j] = i * 10000 + j;
        }
    }

    LOG("Writing data...");

    if (H5Dwrite_multi(NUM_DATASETS, dset_ids, type_ids, sel_space_ids, sel_space_ids, dxpl_id, (const void **)write_data) < 0)
    {
        ERROR("Failed to perform multi write");
    }

    for (size_t i = 0; i < SCROLL_AMNT; i++) {
        fprintf(stderr, "\n");
    }

    LOG("Write complete. Reading data...");
    fflush(stderr);

    if (H5Dread_multi(NUM_DATASETS, dset_ids, type_ids, sel_space_ids, sel_space_ids, dxpl_id, (void **)read_data) < 0)
    {
        ERROR("Failed to perform multi read");
    }

    for (size_t i = 0; i < SCROLL_AMNT; i++) {
        fprintf(stderr, "\n");
    }

    LOG("Verifying read correctness...");

    for (size_t i = 0; i < NUM_DATASETS; i++)
    {
        for (size_t j = 0; j < NUM_INTEGERS; j++)
        {
            if (read_data[i][j] != write_data[i][j]) {
                fprintf(stderr, "At read_data[%zu][%zu], %d != expected %d\n", i, j, read_data[i][j], write_data[i][j]);
                exit(1);
            }
        }
    }

    LOG("Multi read/write complete!")

    LOG("Closing handles...")

    H5Pclose(fapl_id);
    H5Pclose(fcpl_id);
    H5Pclose(dcpl_id);
    H5Pclose(dapl_id);
    H5Pclose(dxpl_id);

    H5Fclose(file_id);

    LOG("Terminating REST VOL...");
    H5rest_term();

    return 0;
}
