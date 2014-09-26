#include <stdio.h>
#include <stdlib.h>

#include "nanomsg/nn.h"
#include "nanomsg/pair.h"
#include "nanomsg/tcp.h"
#include "czmq.h"
#include "util.h"

static void client_task(void *args, zctx_t *ctx, void *pipe)
{
    thr_info *info = (thr_info *) args;
    const char *connect = info->bind_to;
    size_t msg_size = info->msg_size;
    int msg_count = info->msg_count;
    int perf_type = info->perf_type;
    char *buf = (char *) malloc(msg_size);
    memset(buf, 'a', msg_size);

    int client_fd = nn_socket(AF_SP, NN_PAIR);
    assert(client_fd != -1);
    int opt = 1;
    int rc = nn_setsockopt(client_fd, NN_TCP, NN_TCP_NODELAY, &opt, sizeof(opt));
    assert(rc == 0);
    rc = nn_connect(client_fd, connect);
    assert(rc >= 0);

    int i;
    INT64 start_ts, end_ts;
    get_timestamp(&start_ts);
    for (i = 0; i < msg_count; i++) {
        int nbytes = nn_send(client_fd, buf, msg_size, 0);
        assert(nbytes == (int)msg_size);
        if (perf_type == LATENCY) {
            nbytes = nn_recv(client_fd, buf, msg_size, 0);
            assert(nbytes == (int)msg_size);
        }
    }

    buf = (char *) malloc(msg_size);
    nn_recv(client_fd, buf, msg_size, 0);
    assert(strcmp(buf, "well") == 0);
    free(buf);

    get_timestamp(&end_ts);
    if (perf_type == THROUGHPUT) {
        cal_thr(msg_size, msg_count, end_ts - start_ts);
    } else if (perf_type == LATENCY) {
        cal_latency(msg_size, msg_count, end_ts - start_ts);
    }

    zstr_send(pipe, "done");
}

static void *server_task(void *args)
{
    thr_info *info = (thr_info *) args;
    const char *bind_to = info->bind_to;
    size_t msg_size = info->msg_size;
    int msg_count = info->msg_count;
    int perf_type = info->perf_type;

    int srv_fd = nn_socket(AF_SP, NN_PAIR);
    assert(srv_fd != -1);
    int rc = nn_bind(srv_fd, bind_to);
    assert(rc >= 0);

    int req_count = 0;
    size_t nbytes;
    while (true) {
        char *buf = (char *) malloc(msg_size + 1);
        nbytes = nn_recv(srv_fd, buf, msg_size, 0);
        assert(nbytes == (int)msg_size);
        if (perf_type == LATENCY) {
            nbytes = nn_send(srv_fd, buf, msg_size, 0);
            assert(nbytes == (int)msg_size);
        }
        free(buf);

        req_count++;
        if (req_count == msg_count) {
            nbytes = nn_send(srv_fd, "well", 5, 0);
        }
    }

    return NULL;
}

void usage()
{
    fprintf(stderr, "Usage: ./nanomsg_req_perf [OPTIONS]\n");
    fprintf(stderr, "  -b <bind_addr>   \tbind point(eg. tcp://127.0.0.1:12345).\n");
    fprintf(stderr, "  -s <msg_size>    \tmessage size in byte\n");
    fprintf(stderr, "  -c <msg_count>   \tmessage count\n");
    fprintf(stderr, "  -t <test_type>   \ttest type: 0 for throughput, 1 for latency\n");
    fprintf(stderr, "  -h               \tOutput this help and exit.\n");
}

int main(int argc, char *argv[])
{
    char *bind = strdup("tcp://127.0.0.1:12345");
    size_t msg_size = 1024;
    int msg_count = 100;
    int perf_type = THROUGHPUT;

    int opt;
    while ((opt = getopt(argc, argv, "b:s:c:t:h")) != -1) {
        switch (opt) {
        case 'b':
            free(bind);
            bind = strdup(optarg);
            break;
        case 's':
            msg_size = atoi(optarg);
            break;
        case 'c':
            msg_count = atoi(optarg);
            break;
        case 't':
            perf_type = atoi(optarg);
            break;
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
        default:
            fprintf(stderr, "Try 'nanomsg_req_perf -h' for more information\n");
            exit(EXIT_FAILURE);
        }
    }

    thr_info *info = (thr_info *) malloc(sizeof(thr_info));
    info->bind_to = bind;
    info->msg_size = msg_size;
    info->msg_count = msg_count;
    info->perf_type = perf_type;

    zthread_new(server_task, info);
    zctx_t *ctx = zctx_new();
    void *client = zthread_fork(ctx, client_task, info);

    char *signal = zstr_recv(client);
    assert(strcmp(signal, "done") == 0);
    free(signal);

    zctx_destroy(&ctx);
    return 0;
}
