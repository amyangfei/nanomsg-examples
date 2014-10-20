#include <stdio.h>

#include "util.h"
#include "czmq.h"

void serve(int verbose, int recv_port, int send_port)
{
    zctx_t *ctx = zctx_new();
    int rc = -1;

    void *receiver = zsocket_new(ctx, ZMQ_PULL);
    rc = zsocket_bind(receiver, "tcp://*:%d", recv_port);
    assert(rc != -1);

    void *sender = zsocket_new(ctx, ZMQ_PUB);
    rc = zsocket_bind(sender, "tcp://*:%d", send_port);
    assert(rc != -1);

    INT64 messages = 0, last, now;
    get_timestamp(&last);
    while (1) {
        char *msg = zstr_recv(receiver);
        if (!msg) {
            break; // interupt
        }
        rc = zstr_send(sender, msg);
        assert(rc == 0);
        if (verbose) {
            messages += 1;
            get_timestamp(&now);
            if (now - last > 1000) {
                printf("%ld msg/sec\n", messages);
                last = now;
                messages = 0;
            }
        }
    }
}

void usage()
{
    fprintf(stderr, "Usage: ./zmq_pubsub_broker [OPTIONS]\n");
    fprintf(stderr, "  -r <receiver_port>   \treceiver port(default 5562)\n");
    fprintf(stderr, "  -s <sender_port>     \tsender port(default 5561)\n");
    fprintf(stderr, "  -v                   \tverbose mode\n");
    fprintf(stderr, "  -h                   \tOutput this help and exit.\n");
}

int main(int argc, char *argv[])
{
    int verbose = 0;
    int recv_port = 5562;
    int send_port = 5561;

    int opt;
    while ((opt = getopt(argc, argv, "r:s:vh")) != -1) {
        switch (opt) {
        case 'r':
            recv_port = atoi(optarg);
            break;
        case 's':
            send_port = atoi(optarg);
            break;
        case 'v':
            verbose = 1;
            break;
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
        default:
            fprintf(stderr, "Try 'zmq_pubsub_broker -h' for more information");
            exit(EXIT_FAILURE);
        }
    }

    serve(verbose, recv_port, send_port);

    return 0;
}
