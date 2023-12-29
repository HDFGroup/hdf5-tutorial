#include "parse_arguments.hpp"
#include "ou_sampler.hpp"

#include <fstream>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
    size_t path_count, step_count;
    double dt, theta, mu, sigma;
    
    argparse::ArgumentParser program("ou_binary");
    set_options(program);
    program.parse_args(argc, argv);
    get_arguments(program, path_count, step_count, dt, theta, mu, sigma);

    cout << "Running with parameters:"
         << " paths=" << path_count << " steps=" << step_count
         << " dt=" << dt << " theta=" << theta << " mu=" << mu << " sigma=" << sigma << endl;

    vector<double> ou_process;
    ou_sampler(ou_process, path_count, step_count, dt, theta, mu, sigma);
    
    // Write the sample paths to an unformatted binary file

    ofstream file("ou_process.bin", ios::out | ios::binary);
    file.write((char *)&path_count, sizeof(path_count));
    file.write((char *)&step_count, sizeof(step_count));
    file.write((char *)&dt, sizeof(dt));
    file.write((char *)&theta, sizeof(theta));
    file.write((char *)&mu, sizeof(mu));
    file.write((char *)&sigma, sizeof(sigma));
    file.write((char *)ou_process.data(), sizeof(double) * ou_process.size());
    file.close();

    return 0;
}
