# HDFTutorial - Python notebooks

The notebooks in this folder cover the essentials how to use HDF5 in a Python environment, either with h5py and the HDF5 Library or h5pyd and HSDS.

Content is suitable for anyone totally new to HDF5, but even experienced HDF users might pick up a trick or two!

There are various ways to setup your computer environment to run these examples (and your are welcome to use your favorite environment), but the following steps
are relatively easy and should work on Windows, Mac, or Linux systems.

## Download and install VS Code

VS Code is a visual code editor that also runs Jupyter Notebooks.  If you don't have already, download here:
https://code.visualstudio.com/download.

## Get Anaconda Python

Anaconda is a popular Python runtime environment.  With Anaconda you can run different projects in seperate "environments" where each environment can have it's own version of Python and packages.

Download Anaconda here: 
https://www.anaconda.com/download
remo
## Create a Anaconda environment for this tutorial

Run: `conda create -n hdf5tutorial python=3.9`
And then:
     `conda activate hdf5tutorial

## Install Python packages you'll be needing

Run: `conda install h5py` to install h5py, HDF5 library and tools
For running Jupyter, you'll need to run `pip install ipykernel`

If you will be running with HSDS, install hsds and h5pyd:
    `pip install hsds`
    `pip install h5pyd` 

## Start HSDS

If you are planning to run any of the examples with HSDS, run the following in a seperate terminal:

    conda activate hdf5tutorial
    mkdir ~/hsds_data  
    hsds --root_dir ~/hsds_data

Back in your original terminal, configure your credentials for accessing HSDS.  
Run `hsconfigure` and accept the default values at each prompt:

    endpoint: http://localhost:5101
    username: your computer login name
    password: your computer login name
    api_key: none (just press enter)

Press `Y` to save these changes. The file `.hscfg` will be created in your home directory with this information.

Note: This is a basic setup intended for trying out HSDS and not secure.  
For multi-user use and better security options, see the detailed installation 
instructions for HSDS: https://github.com/HDFGroup/hsds/blob/master/README.md 

In order to have the tutorial notebook's use HSDS, change the line `USE_H5PY = True` in the first cell
to `USE_H5PY = False`.  All the notebooks will work with HSDS, but you'll see there can be some differences in the output.

## Open this folder in VS Studio

In VS Studio, select "Open Folder" and go to this folder

## Running the notebooks

If you are not familiar with using Jupyter Notebooks, see: https://code.visualstudio.com/docs/datascience/jupyter-notebooks. 