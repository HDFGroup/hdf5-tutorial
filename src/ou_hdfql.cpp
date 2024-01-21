#include "ou_sampler.hpp"

#include "HDFql.hpp"
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

#define sstr(x) (query.str(""),query.clear(),query << x,query.str().c_str())

int main()
{
    const size_t path_count = 100, step_count = 1000;
    const double dt = 0.01, theta = 1.0, mu = 0.0, sigma = 0.1;

    cout << "Running with parameters:"
         << " paths=" << path_count << " steps=" << step_count
         << " dt=" << dt << " theta=" << theta << " mu=" << mu << " sigma=" << sigma << endl;

    vector<double> ou_process;
    ou_sampler(ou_process, path_count, step_count, dt, theta, mu, sigma);
    
    //
    // Write the sample paths to an HDF5 file using the HDFql C++ bindings!
    //

    HDFql::execute("CREATE TRUNCATE FILE ou_hdfql.h5");
    HDFql::execute("CREATE ATTRIBUTE ou_hdfql.h5 source AS VARCHAR VALUES(\"https://github.com/HDFGroup/hdf5-tutorial\")");

    ostringstream query;
    HDFql::execute(sstr("CREATE DATASET ou_hdfql.h5 \"dataset\" AS DOUBLE(" << path_count << ", " << step_count << ")"));

    HDFql::variableRegister(ou_process.data());
    HDFql::execute(sstr("INSERT INTO ou_hdfql.h5 \"dataset\" VALUES FROM MEMORY " << HDFql::variableGetNumber(ou_process.data())));
    HDFql::variableUnregister(ou_process.data());
    
    HDFql::execute("CREATE ATTRIBUTE ou_hdfql.h5 dataset/comment AS VARCHAR VALUES(\"This dataset contains sample paths of an Ornstein-Uhlenbeck process.\")");
    HDFql::execute("CREATE ATTRIBUTE ou_hdfql.h5 dataset/Wikipedia AS VARCHAR VALUES(\"https://en.wikipedia.org/wiki/Ornstein%E2%80%93Uhlenbeck_process\")");
    HDFql::execute("CREATE ATTRIBUTE ou_hdfql.h5 dataset/rows AS VARCHAR VALUES(\"path\")");
    HDFql::execute("CREATE ATTRIBUTE ou_hdfql.h5 dataset/columns AS VARCHAR VALUES(\"time\")");
    HDFql::execute(sstr("CREATE ATTRIBUTE ou_hdfql.h5 dataset/dt AS DOUBLE VALUES(" << dt << ")"));
    HDFql::execute(sstr("CREATE ATTRIBUTE ou_hdfql.h5 dataset/θ AS DOUBLE VALUES(" << theta << ")"));
    HDFql::execute(sstr("CREATE ATTRIBUTE ou_hdfql.h5 dataset/μ AS DOUBLE VALUES(" << mu << ")"));
    HDFql::execute(sstr("CREATE ATTRIBUTE ou_hdfql.h5 dataset/σ AS DOUBLE VALUES(" << sigma << ")"));

    return 0;
}
