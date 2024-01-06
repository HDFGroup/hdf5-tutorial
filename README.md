# Welcome to the HDF5 Tutorial

<img src="img/HDF5.png" alt="HDF5 logo" title="HDF5 logo" />

[![Open in GitHub Codespaces](https://github.com/codespaces/badge.svg)](https://codespaces.new/HDFGroup/hdf5-tutorial)

*HDF5 is a framework for sharing data.* Its purpose is to frame data so that it can be shared and understood on its own because it speaks for itself; it is *self-describing*. 'Sharing' can mean different things, and there are different ways to achieve this. The framework includes a data model and implementations such as the __[HDF5 library and file format](https://github.com/HDFGroup/hdf5)__ or the __[Highly Scalable Data Service (HSDS)](https://github.com/HDFGroup/hsds)__, and several fantastic community contributions, such as __[`h5py`](https://www.h5py.org/)__, __[HDFql](https://www.hdfql.com/)__, __[`rhdf5`](https://bioconductor.org/packages/release/bioc/html/rhdf5.html)__, or __[H5Web](https://h5web.panosc.eu/)__.

This is a tutorial for new and intermediate HDF5 users. The tutorial is
organized as a series of Jupyter notebooks that are available for download
in a __[GitHub repository](https://github.com/HDFGroup/hdf5-tutorial/)__. The notebooks are intended to be reviewed in the numbered order, but the notebooks are self-contained, and can be visited in any order.

The notebooks in this tutorial are:

1. __[C/C++ 101](./00-CPP-101.ipynb)__ - In case you've never written a C/C++ program
2. __[Theme](./01-Theme.ipynb)__ - A model problem that we will use throughout the tutorial
3. __[Variation 1](./02-Variation1.ipynb)__ - A slightly more complex variant of the problem
4. __[Variation 2](./03-Variation2.ipynb)__ - MPI-parallel HDF5
5. __[Python Bliss](./04-Python-Bliss.ipynb)__ - Where most HDF5 users will spend their time

This tutorial is intended to be executed in a Web browser without installing any software. To this end, this repository comes with a prebuild __[GitHub Codespaces](https://github.com/features/codespaces)__ configuration. All you need is to launch a Codespaces environment and start evaluating the Jupyter notebooks. (You are welcome to clone this repository and set up your local environment. Notice that a few settings are specific to the Codespaces configuration and will need adjusting in a custom environment. We recommend you review [`environment.yml`](./environment.yml) for Python package dependencies, and the files in [`.devcontainer`](./.devcontainer) for additional dependencies.)

Enjoy and let us know what you think about the tutorial in the __[discussion](https://forum.hdfgroup.org/)__! Make it better by reporting __[issues](https://github.com/HDFGroup/hdf5-tutorial/issues)__ or submitting __[pull requests](https://github.com/HDFGroup/hdf5-tutorial/pulls)__!
