#ifndef PARSE_ARGUMENTS1_HPP
#define PARSE_ARGUMENTS1_HPP

#include "argparse.hpp"

// Sets the options for which we are looking
extern void set_options1(argparse::ArgumentParser& program);

// Tests the options and retrieves the arguments
extern int get_arguments1
(
    const argparse::ArgumentParser& program,
    size_t&                         path_count,
    size_t&                         batch_size,
    double&                         dt,
    double&                         theta,
    double&                         mu,
    double&                         sigma
);

#endif
