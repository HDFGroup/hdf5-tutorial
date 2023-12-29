#include "parse_arguments.hpp"
#include "ou_sampler.hpp"

#include <fstream>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
    size_t path_count, step_count;
    double dt, theta, mu, sigma;

    argparse::ArgumentParser program("ou_text");
    set_options(program);
    program.parse_args(argc, argv);
    get_arguments(program, path_count, step_count, dt, theta, mu, sigma);

    cout << "Running with parameters:"
         << " paths=" << path_count << " steps=" << step_count
         << " dt=" << dt << " theta=" << theta << " mu=" << mu << " sigma=" << sigma << endl;

    vector<double> ou_process;
    ou_sampler(ou_process, path_count, step_count, dt, theta, mu, sigma);
    
    // Write the sample paths to a text file
    ofstream file("ou_process.txt");
    
    file << "# paths steps dt theta mu sigma" << endl;
    file << path_count << " " << step_count << " " << dt << " " << theta << " " << mu << " " << sigma << endl;
    file << "# data" << endl;
    
    for (size_t i = 0; i < path_count; ++i)
        {
            for (size_t j = 0; j < step_count; ++j)
            {
                auto pos = i * step_count + j;
                file << ou_process[pos] << " ";
            }
            file << endl;
        }

    file.close();

    return 0;
}
