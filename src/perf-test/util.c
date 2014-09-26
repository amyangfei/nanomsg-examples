#include <sys/time.h>
#include <time.h>
#include <stdio.h>

#include "util.h"

void get_timestamp(INT64 *assigned_timestamp)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    *assigned_timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void cal_thr(size_t msg_sz, int msg_cnt, INT64 time_cost)
{
    uint64_t thr = (uint64_t) ((double) msg_cnt / time_cost * 1000);
    double mbs = (double) (thr * msg_sz) / 1024 / 1024;

    printf("message size: %d [B]\n", (int) msg_sz);
    printf("message count: %d\n", (int) msg_cnt);
    printf("throughput: %d [msg/s]\n", (int) thr);
    printf("throughput: %.3f [Mb/s]\n", mbs);
}

void cal_latency(size_t msg_sz, int msg_cnt, INT64 time_cost)
{
    double latency = (double) time_cost / (msg_cnt * 2);
    printf("message size: %d [B]\n", (int) msg_sz);
    printf("roundtrip count: %d\n", (int) msg_cnt);
    printf("average latency: %.3f [ms]\n", (double) latency);
}
