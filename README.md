# Welcome to the HDF5 Tutorial

<img src="img/HDF5.png" alt="HDF5 logo" title="HDF5 logo" />

[![Open in GitHub Codespaces](https://github.com/codespaces/badge.svg)](https://codespaces.new/HDFGroup/hdf5-tutorial)

*HDF5 is a framework for sharing data.* Its purpose is to frame data so that it can be shared and understood on its own because it speaks for itself; it is *self-describing*. Sharing comes in different forms, for example, via files formatted in a standard way, or by providing access to data through a service endpoints. It is important that these "appearances" cohere and are uniform. For that, the framework includes a data model that is common to all implementations, including the __[HDF5 library and file format](https://github.com/HDFGroup/hdf5)__ or the __[Highly Scalable Data Service (HSDS)](https://github.com/HDFGroup/hsds)__. What makes it all work are fantastic community contributions, such as __[`h5py`](https://www.h5py.org/)__, __[HDFql](https://www.hdfql.com/)__, __[`rhdf5`](https://bioconductor.org/packages/release/bioc/html/rhdf5.html)__, or __[H5Web](https://h5web.panosc.eu/)__.

This is a tutorial for new and intermediate HDF5 users. The tutorial is
organized as a set of Jupyter notebooks that are available for download
in this __[GitHub repository](https://github.com/HDFGroup/hdf5-tutorial/)__. The notebooks are intended to be reviewed in the numbered order, but they are self-contained, and can be visited in any order.

The notebooks in this tutorial are:

1. __[C/C++ 101](./00-CPP-101.ipynb)__ - In case you've never written a C/C++ program
2. __[Theme](./01-Theme.ipynb)__ - A model problem that we will use throughout the tutorial
3. __[Variation 1](./02-Variation1.ipynb)__ - A slightly more complex variant of the problem
4. __[Variation 2](./03-Variation2.ipynb)__ - MPI-parallel HDF5
5. __[Python Bliss](./04-Python-Bliss.ipynb)__ - Where most HDF5 users will spend their time
6. __[PureHDF](./10-PureHDF.ipynb)__ - How to tackle the model problem in C#
7. __[HDFql](./11-HDFql.ipynb)__ - The easy way to manage HDF5 data

This tutorial is intended to be executed in a Web browser without installing any software. To this end, this repository comes with a prebuild __[GitHub Codespaces](https://github.com/features/codespaces)__ configuration. Launch a Codespaces environment by clicking the banner __["Open in GitHub Codespaces"](https://codespaces.new/HDFGroup/hdf5-tutorial)__ and start evaluating the Jupyter notebooks (by placing the cursor into a code cell and pressing `Ctrl+Enter` or `Shift+Enter`). When prompted for a Python kernel, select
```
hdf5-tutorial (Python 3.12.1) /opt/conda/envs/hdf5-tutorial/python
```

You are welcome to clone this repository and set up your local environment. *Notice that a few settings are specific to the Codespaces configuration and will need adjusting in a custom environment.* We recommend you review [`environment.yml`](./environment.yml) for Python package dependencies, and the files in [`.devcontainer`](./.devcontainer) for additional dependencies.

Enjoy and let us know what you think about the tutorial in the __[discussion](https://forum.hdfgroup.org/)__! Please help us improve it by reporting __[issues](https://github.com/HDFGroup/hdf5-tutorial/issues)__ or submitting __[pull requests](https://github.com/HDFGroup/hdf5-tutorial/pulls)__!
