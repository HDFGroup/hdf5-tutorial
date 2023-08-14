# Learning some basic HDF5 API calls

In this tutorial, we will learn how to use some basic methods from the HDF5 API. Specifically, we will go over how to create a file and how to some create attributes of varying types.

## Parsing the arguments

https://github.com/HDFGroup/hdf5-tutorial/blob/f8c8873079892b552fe8a0ca79e41df8df3beeb7/hdf-tutorial.cpp#L18-L79

Because we wish for users to be able to give inputs using the command line, we have to first parse any inputs. Assuming the inputs were properly formatted, we first create a map `args` of type `<string, string>` to hold key and value pairs of arguments. Next, we check if they correspond with the parameters of `path_count`, `step_count`, `dt`, `theta`, `mu`, or `sigma`, which are all used in data generation. If there were no user specified arguments, we will use the default values. Otherwise, we will use the new inputs to carry out the Ornstein-Uhlenbeck process. The values are printed for clarity.

## Generating the sample paths

https://github.com/HDFGroup/hdf5-tutorial/blob/f8c8873079892b552fe8a0ca79e41df8df3beeb7/hdf-tutorial.cpp#L81-L98

Using the Ornstein-Uhlenbeck process, with `path_count` being the number of paths, `step_count` being the number of steps, `dt` being the time step, `theta` being the rate of reversion to the mean, `mu` being the long-term mean of the process, and `sigma` being the volatility of the process, we will generate some data and populate the array `ou_process`.

## Storing the data

Let us look at the following lines of code to learn how to store some data in an HDF5 file. 

https://github.com/HDFGroup/hdf5-tutorial/blob/f8c8873079892b552fe8a0ca79e41df8df3beeb7/hdf-tutorial.cpp#L104-113

We first create an array that specifies the dimensions of the dataspace that will hold the data we just generated in the previous step. Using `{(hsize_t)path_count, (hsize_t)step_count}`, we will name this array `dimsf` and use it in an upcoming step. 

***

We then create a file using `H5Fcreate("ou_process.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT)` called `file`, and we name it `"ou_process.h5"`. The `H5F` prefix lets us know that the method is file related. This file is created with the flag of `H5F_ACC_TRUNC`, which truncates the file, and if it already exists, erases all data previously stored in the file. This newly created file will be where everything -- including the dataset and attributes -- will be stored. It can also be created with a specific file creation property list and file access property list, but we've opted to use the defaults for those arguments.

***

Next, we use `H5Screate_simple(2, dimsf, NULL)` to create a simple dataspace named `space`, and the `S` in the `H5S` prefix tells us that this method has to do with dataspaces. `space` is the dataspace that will hold our dataset. We create it with a rank of 2, and use the previously defined `dimsf`. We could have also creted an array to specify the max size for each dimension, but because we set it to `NULL`, it means that the maximum size is the same as `dimsf`. 

***

We use `H5Dcreate(file, "/dataset", H5T_NATIVE_DOUBLE, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)` to create a dataset of type `H5T_NATIVE_DOUBLE` using `space` as the dataspace and link it to the file that we created earlier. The `D` in `H5D` create means that this method relates to datasets. As with creating the file earlier, we could also specify a link creation property list, dataset cretaion property list, or a dataset access property list, but we are using the defaults for those arguments.

***

We will use `H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, ou_process.data())` to write from a buffer, `ou_process.data()`, of type `H5T_NATIVE_DOUBLE` to `dataset`. The input of `H5S_ALL` is used in place of an identifier for the memory dataspace, which means that the entire file dataspace will be selected for this write. `H5S_ALL` is used again as the identifier of the dataset's dataspace in the file, which means that the file dataspace is used for the memory dataspace, and the entire file dataspace will be selected as well. We could have also specified a dataset transfer property list, but we have left that as `H5P_DEFAULT` again.

***
 
Lastly, we will use `H5Dclose(dataset)` and `H5Sclose(space)` to close `dataset` and `file`.

## Creating some attributes

Because metadata is so important for providing context to data stored in HDF5 files, we will be storing `dt`, `theta`, `mu`, and `sigma` as different types of attributes to give some examples of to use attributes. 

Note, we will be showing you ways to storing our double attributes as attributes of different types (such as float, scalar UTF-8 encoded string attribute, or even an element in a 3D array), but it's not recommended to do so. In fact, it's better to keep it simple and follow common conventions. We do so only to show some of the different types of attributes that are possible.

In the following lines of code, we will create a attribute of type double. 

https://github.com/HDFGroup/hdf5-tutorial/blob/f8c8873079892b552fe8a0ca79e41df8df3beeb7/hdf-tutorial.cpp#L117-L123

We first use `H5Screate(H5S_SCALAR)` to create a dataspace called `dt_attr_space`. Since it is created using the type of `H5S_SCALAR`, it has a single element. This single element could be a complex datatype, however, like a compound or array datatype. Next, using `H5Acreate_by_name(file, "dataset", "dt", H5T_NATIVE_DOUBLE, dt_attr_space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)`, we create an attribute `dt_attr`. This attribute is located in `file`, attached to `\dataset`, named `"dt"`, of type `H5T_NATIVE_DOUBLE`, uses the dataspace of `dt_attr_space`, and does not have a specified attribute creation property list, attribute access property list, or link access property list. Lastly, we use `H5Awrite(dt_attr, H5T_NATIVE_DOUBLE, &dt)` to write the data of type `H5T_NATIVE_DOUBLE` from `&dt` to the file using the attribute identifier of `dt_attr`. 

***

Next, we will store `theta` of type `double` as an attribute of type `float`. 

https://github.com/HDFGroup/hdf5-tutorial/blob/f8c8873079892b552fe8a0ca79e41df8df3beeb7/hdf-tutorial.cpp#L125-L131

The only difference here from the previous step is that we create an attribute of type `H5T_NATIVE_FLOAT` in `H5Acreate_by_name(file, "dataset", "theta", H5T_NATIVE_FLOAT, theta_attr_space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)`, and although we write from the buffer as type double in the next step, `H5Awrite(theta_attr, H5T_NATIVE_DOUBLE, &theta)`, it is stored as type `float` due to implicit conversion. 

***

Next, we can try something a bit funky and store `mu` as a scalar UTF-8 encoded string attribute.

https://github.com/HDFGroup/hdf5-tutorial/blob/f8c8873079892b552fe8a0ca79e41df8df3beeb7/hdf-tutorial.cpp#L135-L146

We first convert `mu` from a double to a string `mu_str` using `to_string(mu)`. As before, we create `mu_attr_space` using `H5Screate(H5S_SCALAR)`. Using `H5Tcopy(H5T_C_S1)`, we return a datatype of `atype`, and we use this datatype to define the UTF-8 encoded string type that we will store `mu` as. Specifically, we can do this using `H5Tset_size(atype, mu_str.length())` and `H5Tset_cset(atype, H5T_CSET_UTF8)`. This will set the size of the datatype and the character set used. After these steps, we will create the attribute and write it like we did earlier. 

***

As mentioned above, you can even store arrays as attributes! 

https://github.com/HDFGroup/hdf5-tutorial/blob/f8c8873079892b552fe8a0ca79e41df8df3beeb7/hdf-tutorial.cpp#L148-L156

We can first create a simple attribute dataspace `sigma_attr_space` and define an array `adim` to be of size `{1,1,1}` to be used later. Using `H5Sset_extent_simple(sigma_attr_space, 3, adim, NULL)`, we can resize the existing dataspace to a rank of 3. Now, when we write the attribute using `H5Acreate_by_name(file, "dataset", "sigma", H5T_NATIVE_DOUBLE, sigma_attr_space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)`, it is written as the single element in a 1 by 1 by 1 array. 

Alteratively, we can store the volatility in a scalar 3D array attribute.

https://github.com/HDFGroup/hdf5-tutorial/blob/f8c8873079892b552fe8a0ca79e41df8df3beeb7/hdf-tutorial.cpp#L158-L18

This time, we create a scalar attribute dataspace `sigma_attr_space` and define a type `atype` using `H5Tarray_create(H5T_NATIVE_DOUBLE, 3, adim)`. This time, when we write the attribute using `H5Acreate_by_name(file, "dataset", "sigma1", atype, sigma_attr_space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)`, we use the newly defined scalar datatype instead. 