#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>

#include "util.h"
#include "nanomsg/nn.h"
#include "nanomsg/pipeline.h"
#include "nanomsg/pubsub.h"
#include "nanomsg/tcp.h"

void serve(int verbose, int recv_port, int send_port)
{
    char recv_addr[SIZE32], send_addr[SIZE32];
    memset(recv_addr, '\0', SIZE32);
    memset(send_addr, '\0', SIZE32);
    sprintf(recv_addr, "tcp://*:%d", recv_port);
    sprintf(send_addr, "tcp://*:%d", send_port);

    int rc, recv_fd, send_fd;

    recv_fd = nn_socket(AF_SP, NN_PULL);
    assert(recv_fd != -1);
    rc = nn_bind(recv_fd, recv_addr);
    assert(rc >= 0);

    send_fd = nn_socket(AF_SP, NN_PUB);
    assert(send_fd != -1);
    rc = nn_bind(send_fd, send_addr);
    assert(rc >= 0);

    INT64 messages = 0, last, now;
    get_timestamp(&last);
    while (1) {
        char *buf = NULL;
        int bytes = nn_recv(recv_fd, &buf, NN_MSG, 0);
        assert(bytes > 0);

        bytes = nn_send(send_fd, buf, bytes, 0);

        nn_freemsg(buf);

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
    fprintf(stderr, "Usage: ./nanomsg_pubsub_broker [OPTIONS]\n");
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
            fprintf(stderr, "Try 'nanomsg_pubsub_broker -h' for more information");
            exit(EXIT_FAILURE);
        }
    }

    serve(verbose, recv_port, send_port);

    return 0;
}
