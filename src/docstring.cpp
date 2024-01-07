#include "docstring.hpp"

using namespace std;

void add_docstring(hid_t& loc, const string& rel, const string& key, const string& val)
{
    hid_t scalar = H5Screate(H5S_SCALAR), strtype = H5Tcopy(H5T_C_S1);
    H5Tset_size(strtype, val.size());
    H5Tset_strpad(strtype, H5T_STR_NULLTERM);
    hid_t attr = H5Acreate_by_name(loc, (rel.size() >= 1) ? rel.c_str() : ".", key.c_str(), strtype, scalar,
        H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attr, strtype, val.c_str());
    H5Aclose(attr);
    H5Tclose(strtype);
    H5Sclose(scalar);
}
