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

## Create a Anaconda environment for this tutorial

Run: `conda create -n hdf5tutorial python=3.9`
And then:
     `conda activate hdf5tutorial

## Install Python packages you'll be needing

Run: `pip install -f requirements.txt`

## Start HSDS

If you are planning to run any of the examples with HSDS, run the following in a seperate terminal:

    conda activate hdf5tutorial
    mkdir ~/hsds_data  
    hsds --root_dir ~/hsds_data

In order to have the tutorial notebook's use HSDS, change the line `USE_H5PY = True` in the first cell
to `USE_H5PY = False`.  All the notebooks will work with HSDS, but you'll see there can be some differences
in the output.

## Open this folder in VS Studio

In VS Studio, select "Open Folder" and go to this folder

## Running the notebooks

TBD: basic howto for Jupyter