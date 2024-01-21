#include "ou_sampler.hpp"

#include "HDFql.hpp"
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

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

    cout << "HDFql version: " << HDFql::Version << endl;

    HDFql::execute("CREATE TRUNCATE FILE ou_hdfql.h5");
    HDFql::execute("CREATE ATTRIBUTE ou_hdfql.h5 source AS VARCHAR VALUES(\"https://github.com/HDFGroup/hdf5-tutorial\")");

    std::ostringstream query;
    query << "CREATE DATASET ou_hdfql.h5 \"dataset\" AS DOUBLE(" << path_count << ", " << step_count << ")";
    HDFql::execute(query.str().c_str());

    HDFql::variableRegister(ou_process.data());
    query.str("");
    query << "INSERT INTO ou_hdfql.h5 \"dataset\" VALUES FROM MEMORY " << HDFql::variableGetNumber(ou_process.data());
    HDFql::execute(query.str().c_str());
    //HDFql::execute("INSERT INTO ou_hdfql.h5 \"dataset\" VALUES FROM MEMORY 0");
    HDFql::variableUnregister(ou_process.data());
    
    HDFql::execute("CREATE ATTRIBUTE ou_hdfql.h5 dataset/comment AS VARCHAR VALUES(\"This dataset contains sample paths of an Ornstein-Uhlenbeck process.\")");
    HDFql::execute("CREATE ATTRIBUTE ou_hdfql.h5 dataset/Wikipedia AS VARCHAR VALUES(\"https://en.wikipedia.org/wiki/Ornstein%E2%80%93Uhlenbeck_process\")");
    HDFql::execute("CREATE ATTRIBUTE ou_hdfql.h5 dataset/rows AS VARCHAR VALUES(\"path\")");
    HDFql::execute("CREATE ATTRIBUTE ou_hdfql.h5 dataset/columns AS VARCHAR VALUES(\"time\")");

    query.str("");
    query << "CREATE ATTRIBUTE ou_hdfql.h5 dataset/dt AS DOUBLE VALUES(" << dt << ")";
    HDFql::execute(query.str().c_str());

    query.str("");
    query << "CREATE ATTRIBUTE ou_hdfql.h5 dataset/θ AS DOUBLE VALUES(" << theta << ")";
    HDFql::execute(query.str().c_str());

    query.str("");
    query << "CREATE ATTRIBUTE ou_hdfql.h5 dataset/μ AS DOUBLE VALUES(" << mu << ")";
    HDFql::execute(query.str().c_str());

    query.str("");
    query << "CREATE ATTRIBUTE ou_hdfql.h5 dataset/σ AS DOUBLE VALUES(" << sigma << ")";
    HDFql::execute(query.str().c_str());

    return 0;
}
