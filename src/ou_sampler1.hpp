#ifndef OU_SAMPLER1_HPP
#define OU_SAMPLER1_HPP

#include "hdf5.h"
#include <vector>

// Creates `batch_size` sample paths of random length with parameters
// `dt`, `theta`, `mu`, and `sigma`
extern void ou_sampler1
(
    std::vector<double>&  ou_process,
    std::vector<hsize_t>& offset,
    const size_t&         batch_size,
    const double&         dt,
    const double&         theta,
    const double&         mu,
    const double&         sigma
);

#endif
