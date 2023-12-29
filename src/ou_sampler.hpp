#ifndef OU_SAMPLER_HPP
#define OU_SAMPLER_HPP

#include <vector>

// Creates `path_count` sample paths of length `step_count` with parameters
// `dt`, `theta`, `mu`, and `sigma`
extern void ou_sampler
(
    std::vector<double>& ou_process,
    const size_t&        path_count,
    const size_t&        step_count,
    const double&        dt,
    const double&        theta,
    const double&        mu,
    const double&        sigma
);

#endif
