#include "partitioner.hpp"

using namespace std;

void partition_work(size_t path_count, int rank, int nranks, size_t &start, size_t &stop)
{
  size_t count = path_count / nranks;
  size_t remainder = path_count % nranks;

  if ((size_t)rank < remainder) {
    // The first 'remainder' ranks get 'count + 1' tasks each
    start = rank * (count + 1);
    stop = start + count;
  } else {
    // The remaining 'nranks - remainder' ranks get 'count' task each
    start = rank * count + remainder;
    stop = start + (count - 1);
  }
}
