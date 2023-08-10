# Handling Sample Paths of Random Lengths and Writing Data in Batches

In this tutorial, we will learn how to handle sample paths of random lengths and write data in batches. We will use the same data as in the previous tutorial, however, we must adjust for the fact that the sample paths are of random lengths. We will also learn how to write data in batches, which is useful when the data is too large to fit in memory.

## Parsing the arguments

Parsing the arguments is the same as in the previous tutorial. The only difference is that there is no need to specify the number of time steps, which is now generated at random (from a uniform distribution over `[1, USHRT_MAX]`). It is replaced by the batch size, which is the number of sample paths to be generated at each iteration.

## Generating the sample paths

For each batch, we maintain two vectors to track the sample paths and their lengths.

https://github.com/HDFGroup/hdf5-tutorial/blob/07eb4b4035aedcec0fcde14f6f673c252f132a9b/hdf-tutorial.1.cpp#L105C1-L107C34

The `offset` vector tracks the length of the sample paths, which are stored consecutively in `ou_process`.

For each batch, the sample path generation is the same as in the previous tutorial.

https://github.com/HDFGroup/hdf5-tutorial/blob/07eb4b4035aedcec0fcde14f6f673c252f132a9b/hdf-tutorial.1.cpp#L119C1-L135C10

## Storing the data

However we choose to map the sample paths to one or more HDF5 datasets, because the sample path lenghts are not know in advance, we must deal with the fact that certain dataset dimensions are not known in advance. We will use the `H5Dset_extent` function to extend the dataset dimensions as needed.

It is common to use two datasets to store the sample paths and their lengths. The first dataset stores the sample paths, and the second dataset stores their lengths as offsets into the first dataset. We call the first dataset `/paths` and the second dataset `/descr`. Both are one-dimensional. The `/paths` dataset is of variable length, and the `/descr` dataset is of fixed length. The `/descr` dataset is of fixed length because it is a vector of offsets, and the number of offsets equals the number of paths, and is known in advance.

### The `/paths` dataset

https://github.com/HDFGroup/hdf5-tutorial/blob/07eb4b4035aedcec0fcde14f6f673c252f132a9b/hdf-tutorial.1.cpp#L84C1-L92C21

Because of its indefinite size, the `/paths` dataset is created with the `H5S_UNLIMITED` flag. The `H5S_UNLIMITED` flag is used to create a dataspace of indefinite size. The `H5S_UNLIMITED` flag is used in conjunction with the `H5Dset_extent` function to extend the dataset dimensions as needed. Datasets of indefinite size must use chunked storage layout for incremental allocation in the HDF5 file. In this example, we use a chunk size of 1 MiB.

### The `/descr` dataset

https://github.com/HDFGroup/hdf5-tutorial/blob/07eb4b4035aedcec0fcde14f6f673c252f132a9b/hdf-tutorial.1.cpp#L94C1-L98C21

The `/descr` dataset is of fixed size. Its number of elements equals (total) the number of sample paths. To determine the length and elements of the `n`-ths path, we look up the `n`-th and `n+1`-th elements of the `/descr` dataset. The difference is the length of the `n`-th path. The actual samples of the `n`-th path are stored in the `/paths` dataset, starting at the `n`-th element.

### Writing the data

Writing the `/descr` dataset is straightforward. Writing the `/paths` dataset is a little more interesting.

https://github.com/HDFGroup/hdf5-tutorial/blob/07eb4b4035aedcec0fcde14f6f673c252f132a9b/hdf-tutorial.1.cpp#L140C1-L154C34

It is done in several steps as follows:

1. Retrieve the current dataset dimension. (1D)
2. Extend the dataset dimension by the paths in the batch.
3. Retrieve the updated dataset dimension.
4. Select the hyperslab corrsponding to the batch.
5. Write the batch to the hyperslab.

The third step is crucial. Without it, the hyperslab selection would be incorrect. The hyperslab selection is relative to the current dataset dimension, not the original dataset dimension. The original dataset dimension is the dimension after the previous batch. The dataset dimension is updated as a side effect by the `H5Dset_extent` function.

### Adding attributes

As in the previous examples, we add attributes, albeit, this time, to the root group.

https://github.com/HDFGroup/hdf5-tutorial/blob/07eb4b4035aedcec0fcde14f6f673c252f132a9b/hdf-tutorial.1.cpp#L177C1-L190C26