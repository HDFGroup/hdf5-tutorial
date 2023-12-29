
#include "ou_sampler1.hpp"
#include <random>

using namespace std;

void ou_sampler1
(
    vector<double>&  ou_process,
    vector<hsize_t>& offset,
    const size_t&    batch_size,
    const double&    dt,
    const double&    theta,
    const double&    mu,
    const double&    sigma
)
{
    // Store sample paths in one contiguous buffer
    ou_process.clear();
    offset.clear();
    offset.push_back(0);

    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<unsigned int> path_len_dist(1, USHRT_MAX);  // path length is between 1 and 65535
    normal_distribution<double> dist(0.0, sqrt(dt));  // N(0, dt)

    size_t pos = 0;  // offset into the ou_process vector

    // Generate a batch of paths and offsets
    for (size_t i = 0; i < batch_size; ++i)
    {
        // Generate random path length
        size_t step_count = path_len_dist(generator);
        // Resize the vector to make room for the new path
        ou_process.resize(pos + step_count);  
            
        // Generate the path
        ou_process[pos] = 0; // start at x = 0
        for (size_t j = 1; j < step_count; ++j)
        {
            auto dW = dist(generator);
            ++pos;  // advance the offset
            ou_process[pos] = ou_process[pos - 1] + theta * (mu - ou_process[pos - 1]) * dt + sigma * dW;
        }

        // This is the offset of the next path
        offset.push_back(offset.back() + (hsize_t)step_count);  
    }
}
