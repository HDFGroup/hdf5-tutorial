#include "restvol.h"
#include "hdf5.h"
#include "rest_vol_public.h"

#include <stdlib.h>

int main(void)
{
    LOG("Initializing REST VOL...")
    H5rest_init();

    // Set a file access property to use the REST VOL
    hid_t fapl = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_fapl_rest_vol(fapl);

    LOG("Creating file...")
    hid_t file = H5I_INVALID_HID;
    if ((file = H5Fcreate(HSDS_FILENAME_PREFIX "my_file.h5", H5F_ACC_TRUNC, H5P_DEFAULT, fapl)) < 0)
    {
        ERROR("Failed to create file");
    }

    LOG("Setting up data...");
    fprintf(stderr, "  %d Datasets of %d integers each\n", NUM_DATASETS, NUM_INTEGERS);

    int *write_data[NUM_DATASETS];
    int *read_data[NUM_DATASETS];

    hid_t dspace_ids[NUM_DATASETS];
    hid_t dset_ids[NUM_DATASETS];
    hid_t type_ids[NUM_DATASETS];
    hid_t sel_space_ids[NUM_DATASETS];

    for (size_t i = 0; i < NUM_DATASETS; i++)
    {
        dspace_ids[i] = H5Screate_simple(1, (hsize_t[]){NUM_INTEGERS}, NULL);

        char dset_name[FILENAME_BUFFER_SIZE];
        snprintf(dset_name, FILENAME_BUFFER_SIZE, "%s%zu", "dset", i);

        if ((dset_ids[i] = H5Dcreate(file, dset_name, H5T_NATIVE_INT, dspace_ids[i],
                                     H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0)
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
            write_data[i][j] = i * 10000 + j;
    }

    LOG("Writing data...");

    if (H5Dwrite_multi(NUM_DATASETS, dset_ids, type_ids, sel_space_ids, sel_space_ids,
                       H5P_DEFAULT, (const void **)write_data) < 0)
    {
        ERROR("Failed to perform multi write");
    }

    for (size_t i = 0; i < SCROLL_AMNT; i++)
        fprintf(stderr, "\n");

    LOG("Write complete. Reading data...");
    fflush(stderr);

    if (H5Dread_multi(NUM_DATASETS, dset_ids, type_ids, sel_space_ids, sel_space_ids,
                      H5P_DEFAULT, (void **)read_data) < 0)
    {
        ERROR("Failed to perform multi read");
    }

    for (size_t i = 0; i < SCROLL_AMNT; i++)
        fprintf(stderr, "\n");

    LOG("Verifying read correctness...");

    for (size_t i = 0; i < NUM_DATASETS; i++)
        for (size_t j = 0; j < NUM_INTEGERS; j++)
        {
            if (read_data[i][j] != write_data[i][j])
            {
                fprintf(stderr, "At read_data[%zu][%zu], %d != expected %d\n",
                        i, j, read_data[i][j], write_data[i][j]);
                exit(1);
            }
        }

    LOG("Multi read/write complete!")

    H5Pclose(fapl);
    H5Fclose(file);

    LOG("Terminating REST VOL...");
    H5rest_term();

    return 0;
}
