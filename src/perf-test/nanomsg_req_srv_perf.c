#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <getopt.h>

#include "nanomsg/nn.h"
#include "nanomsg/pair.h"
#include "nanomsg/tcp.h"
#include "util.h"

#define INIT_MSG_SIZE   1024

int srv_stop = 0;

void server_stop(int signum)
{
    if (signum == SIGTERM) {
        printf("stopping srv...\n");
        srv_stop = 1;
    }
}

void *server_task(char *bind_to, int test_type)
{
    int srv_fd = nn_socket(AF_SP, NN_PAIR);
    assert(srv_fd != -1);
    int rc = nn_bind(srv_fd, bind_to);
    assert(rc >= 0);

    size_t nbytes;
    /* When we call nn_recv (int s, void *buf, size_t len, int flags)
     * any bytes exceeding the length specified by the len argument will be
     * truncated. We record the last received msg size as next recv buf size*/
    size_t last_size = INIT_MSG_SIZE;
    while (!srv_stop) {
        char *buf = (char *) malloc(last_size);
        nbytes = nn_recv(srv_fd, buf, last_size, 0);
        assert(nbytes > 0);
        last_size = nbytes;
        if (test_type == LATENCY) {
            nbytes = nn_send(srv_fd, buf, last_size, 0);
            assert(nbytes == (int)last_size);
        }
        free(buf);
    }

    return NULL;
}

void usage()
{
    fprintf(stderr, "Usage: ./nanomsg_req_srv_perf [OPTIONS]\n");
    fprintf(stderr, "  -b <bind_addr>   \tbind point(eg. tcp://127.0.0.1:12345).\n");
    fprintf(stderr, "  -t <test_type>   \ttest type: 0 for throughput, 1 for latency\n");
    fprintf(stderr, "  -h               \tOutput this help and exit.\n");
}

int main(int argc, char *argv[])
{
    char *bind = strdup("tcp://127.0.0.1:12345");
    int perf_type = THROUGHPUT;

    int opt;
    while ((opt = getopt(argc, argv, "b:t:h")) != -1) {
        switch (opt) {
        case 'b':
            free(bind);
            bind = strdup(optarg);
            break;
        case 't':
            perf_type = atoi(optarg);
            break;
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
        default:
            fprintf(stderr, "Try 'nanomsg_req_srv_perf -h' for more information\n");
            exit(EXIT_FAILURE);
        }
    }

    signal(SIGTERM, server_stop);

    server_task(bind, perf_type);

    return 0;
}
