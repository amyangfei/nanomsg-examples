#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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
    double mbs = (double) (thr * msg_sz * 8) / 1000 / 1000;

    printf("message size: %d [B]\n", (int) msg_sz);
    printf("message count: %d\n", (int) msg_cnt);
    printf("time cost: %lu ms\n", time_cost);
    printf("throughput: %d [msg/s]\n", (int) thr);
    printf("throughput: %.3f [Mb/s]\n", mbs);
}

void cal_latency(size_t msg_sz, int msg_cnt, INT64 time_cost)
{
    double latency = (double) time_cost / (msg_cnt * 2);
    printf("message size: %d [B]\n", (int) msg_sz);
    printf("time cost: %lu ms\n", time_cost);
    printf("roundtrip count: %d\n", (int) msg_cnt);
    printf("average latency: %.3f [ms]\n", (double) latency);
}

/*
 * code comes from http://stackoverflow.com/a/4586990/1115857
 */
int get_cpu_count() {
    long nprocs = -1;
    long nprocs_max = -1;
#ifdef _SC_NPROCESSORS_ONLN
    nprocs = sysconf(_SC_NPROCESSORS_ONLN);
    if (nprocs < 1) {
        fprintf(stderr, "Could not determine number of CPUs online:\n%s\n",
                strerror (errno));
        return -1;
    }
    nprocs_max = sysconf(_SC_NPROCESSORS_CONF);
    if (nprocs_max < 1) {
        fprintf(stderr, "Could not determine number of CPUs configured:\n%s\n",
                strerror (errno));
        return -1;
    }
    /*printf ("%ld of %ld processors online\n",nprocs, nprocs_max);*/
    return nprocs;
#else
    fprintf(stderr, "Could not determine number of CPUs");
    return -1;
#endif
}

static int comp_int(const void * elem1, const void * elem2)
{
    int f = *((int*)elem1);
    int s = *((int*)elem2);
    if (f > s) return  1;
    if (f < s) return -1;
    return 0;
}

int get_median(int *stats, int len)
{
    qsort(stats, len, sizeof(*stats), comp_int);
    if (len <= 5) {
        return stats[len / 2];
    } else {
        int sum = 0, i;
        for (i = 2; i < len - 2; i++) {
            sum += stats[i];
        }
        return sum / (len - 4);
    }
}

