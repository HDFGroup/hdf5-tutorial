#ifndef RESTVOL_H
#define RESTVOL_H

#define HSDS_FILENAME_PREFIX "/home/test_user1/"

#define FILENAME_BUFFER_SIZE 1024
#define NUM_DATASETS 2
#define NUM_INTEGERS (1000 * 1000 * 100) / NUM_DATASETS

#define ERROR(msg)                \
    fprintf(stderr, "%s\n", msg); \
    exit(1);

#define LOG(msg) \
    fprintf(stderr, "%s\n", msg);

// For displaying log messages after the progress bars for dataset read/write
#define SCROLL_AMNT (NUM_DATASETS < 21 ? NUM_DATASETS : 21)

#endif
