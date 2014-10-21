#ifndef __UTIL_H
#define __UTIL_H

#include <stdio.h>
#include <stdint.h>

#define INT64 int64_t

#define THROUGHPUT  0
#define LATENCY     1

#define SIZE4       4
#define SIZE8       8
#define SIZE16      16
#define SIZE32      32
#define SIZE64      64
#define SIZE1024    1024

typedef struct thr_info {
    char *bind_to;
    size_t msg_size;
    int msg_count;
    int perf_type;
} thr_info;

typedef struct pubsub_info {
    char *broker_ip;
    int recv_port;
    int send_port;

    int msg_size;
    int num_channels;
    char **channels;

    int verbose;
} pubsub_info;

void get_timestamp(INT64 *assigned_timestamp);
void cal_thr(size_t msg_sz, int msg_cnt, INT64 time_cost);
void cal_latency(size_t msg_sz, int msg_cnt, INT64 time_cost);
int get_cpu_count();
int get_median(int *stats, int len);

#endif
