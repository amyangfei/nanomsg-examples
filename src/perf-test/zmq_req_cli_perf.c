#include <stdlib.h>
#include <pthread.h>

#include "czmq.h"
#include "util.h"

static void client_task(void *args, zctx_t *ctx, void *pipe)
{
    thr_info *info = (thr_info *) args;
    const char *connect = info->bind_to;
    size_t msg_size = info->msg_size;
    int msg_count = info->msg_count;
    char *buf = (char *) malloc(msg_size+1);
    memset(buf, 'a', msg_size);
    buf[msg_size] = '\0';

    void *client = zsocket_new(ctx, ZMQ_DEALER);
    zmq_connect(client, connect);

    int i;
    INT64 start_ts, end_ts;
    get_timestamp(&start_ts);
    for (i = 0; i < msg_count; i++) {
        int sent = zstr_send(client, buf);
        assert(sent == 0);
        char *recv_msg = zstr_recv(client);
        assert(strlen(recv_msg) == msg_size);
        free(recv_msg);
    }
    free(buf);

    get_timestamp(&end_ts);
    cal_thr(msg_size, msg_count, end_ts - start_ts);

    zstr_send(pipe, "done");
}

void usage()
{
    fprintf(stderr, "Usage: ./zmq_req_cli_perf [OPTIONS]\n");
    fprintf(stderr, "  -b <conn_addr>   \tconnect point(eg. tcp://127.0.0.1:12345).\n");
    fprintf(stderr, "  -s <msg_size>    \tmessage size in byte\n");
    fprintf(stderr, "  -c <msg_count>   \tmessage count\n");
    fprintf(stderr, "  -h               \tOutput this help and exit.\n");
}

int main(int argc, char *argv[])
{
    char *conn_to = strdup("tcp://127.0.0.1:12345");
    size_t msg_size = 1024;
    int msg_count = 100;
    int perf_type = THROUGHPUT; // not used

    int opt;
    while ((opt = getopt(argc, argv, "b:s:c:h")) != -1) {
        switch (opt) {
        case 'b':
            free(conn_to);
            conn_to = strdup(optarg);
            break;
        case 's':
            msg_size = atoi(optarg);
            break;
        case 'c':
            msg_count = atoi(optarg);
            break;
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
        default:
            fprintf(stderr, "Try 'zmq_req_cli_perf -h' for more information");
            exit(EXIT_FAILURE);
        }
    }

    thr_info *info = (thr_info *) malloc(sizeof(thr_info));
    info->bind_to = conn_to;
    info->msg_size = msg_size;
    info->msg_count = msg_count;
    info->perf_type = perf_type;

    zctx_t *ctx = zctx_new();
    void *client = zthread_fork(ctx, client_task, info);

    char *signal = zstr_recv(client);
    assert(strcmp(signal, "done") == 0);
    free(signal);

    zctx_destroy(&ctx);
    return 0;
}
