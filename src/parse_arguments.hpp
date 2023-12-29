#ifndef PARSE_ARGUMENTS_HPP
#define PARSE_ARGUMENTS_HPP

#include "argparse.hpp"

// Sets the options for which we are looking
extern void set_options(argparse::ArgumentParser& program);

// Tests the options and retrieves the arguments
extern int get_arguments
(
    const argparse::ArgumentParser& program,
    size_t&                         path_count,
    size_t&                         step_count,
    double&                         dt,
    double&                         theta,
    double&                         mu,
    double&                         sigma
);

#endif
