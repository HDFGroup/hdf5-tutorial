
#ifndef PARTITIONER_HPP
#define PARTITIONER_HPP

#include <cstddef>

extern void partition_work(std::size_t path_count, int rank, int nranks, std::size_t &start, std::size_t &stop);

#endif
