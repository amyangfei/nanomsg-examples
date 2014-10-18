#include <stdlib.h>

#include "util.h"
#include "czmq.h"

void usage()
{
    fprintf(stderr, "Usage: ./zmq_req_srv_perf [OPTIONS]\n");
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
            fprintf(stderr, "Try 'zmq_req_srv_perf -h' for more information");
            exit(EXIT_FAILURE);
        }
    }

    zctx_t *ctx = zctx_new();
    void *server = zsocket_new(ctx, ZMQ_DEALER);
    int rc = zsocket_bind(server, "%s", bind);
    assert(rc != -1);

    while (true) {
        char *msg = zstr_recv(server);
        if (!msg) {
            break;  //Interrupted
        }
        if (perf_type == LATENCY) {
            zstr_send(server, msg);
        }
        free(msg);
    }

    zctx_destroy(&ctx);

    return 0;
}
