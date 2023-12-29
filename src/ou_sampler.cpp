
#include <random>

using namespace std;

void ou_sampler
(
    vector<double>& ou_process,
    const size_t&   path_count,
    const size_t&   step_count,
    const double&   dt,
    const double&   theta,
    const double&   mu,
    const double&   sigma
)
{
    // Store sample paths in one contiguous buffer
    ou_process.clear();
    ou_process.resize(path_count * step_count);

    random_device rd;
    mt19937 generator(rd());
    normal_distribution<double> dist(0.0, sqrt(dt));

    for (size_t i = 0; i < path_count; ++i)
    {
        ou_process[i * step_count] = 0; // sample paths start at x = 0
        for (size_t j = 1; j < step_count; ++j)
        {
            auto dW = dist(generator);
            auto pos = i * step_count + j;
            ou_process[pos] = ou_process[pos - 1] + theta * (mu - ou_process[pos - 1]) * dt + sigma * dW;
        }
    }
}
