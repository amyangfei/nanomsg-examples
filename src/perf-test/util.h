#ifndef __UTIL_H
#define __UTIL_H

#include <stdio.h>
#include <stdint.h>

#define INT64 int64_t

#define THROUGHPUT  0
#define LATENCY     1

typedef struct thr_info {
    char *bind_to;
    size_t msg_size;
    int msg_count;
    int perf_type;
} thr_info;

void get_timestamp(INT64 *assigned_timestamp);
void cal_thr(size_t msg_sz, int msg_cnt, INT64 time_cost);
void cal_latency(size_t msg_sz, int msg_cnt, INT64 time_cost);

#endif
