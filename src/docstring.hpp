#ifndef DOCSTRING_HPP
#define DOCSTRING_HPP

#include "hdf5.h"
#include <string>

// Writes a scalar HDF5 string attribute
extern void add_docstring
(
    hid_t&             loc,
    const std::string& rel,
    const std::string& key,
    const std::string& val);

#endif
