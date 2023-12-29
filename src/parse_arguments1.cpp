#include "parse_arguments1.hpp"
#include <cfloat>
#include <iostream>

using namespace std;

void set_options1(argparse::ArgumentParser& program)
{
    program.add_argument("-p", "--paths")  // command line arguments
    .help("chooses the numnber of paths")  // synopsis
    .default_value(size_t{100})            // default value
    .scan<'u', size_t>();                  // expected type

    program.add_argument("-b", "--batch")
    .help("chooses the batch size")
    .default_value(size_t{100})
    .scan<'u', size_t>();

    program.add_argument("-d", "--dt")
    .help("chooses the time step")
    .default_value(double{0.01})
    .scan<'f', double>();

    program.add_argument("-t", "--theta")
    .help("chooses the rate of reversion to the mean")
    .default_value(double{1.0})
    .scan<'f', double>();

    program.add_argument("-m", "--mu")
    .help("chooses the long-term mean of the process")
    .default_value(double{0.0})
    .scan<'f', double>();

    program.add_argument("-g", "--sigma")
    .help("chooses the volatility of the process")
    .default_value(double{0.1})
    .scan<'f', double>();
}

int get_arguments1
(
    const argparse::ArgumentParser& program,
    size_t&                         path_count,
    size_t&                         batch_size,
    double&                         dt,
    double&                         theta,
    double&                         mu,
    double&                         sigma
)
{
    path_count = program.get<size_t>("--paths");
    if (path_count == 0) {
        cerr << "Number of paths must be greater than zero" << endl;
        return -1;
    }
    batch_size = program.get<size_t>("--batch");
    if (batch_size == 0) {
        cerr << "Batch size must be greater than zero" << endl;
        return -1;
    }
    dt = program.get<double>("--dt");
    if (dt < DBL_MIN) {
        cerr << "Time step must be greater than zero" << endl;
        return -1;
    }
    theta = program.get<double>("--theta");
    if (theta < DBL_MIN) {
        cerr << "Reversion rate must be greater than zero" << endl;
        return -1;
    }
    mu = program.get<double>("--mu");
    sigma = program.get<double>("--sigma");
    if (sigma < DBL_MIN) {
        cerr << "Volatility must be greater than zero" << endl;
        return -1;
    }

    return 0;
}
