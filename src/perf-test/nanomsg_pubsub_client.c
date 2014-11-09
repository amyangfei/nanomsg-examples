#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "util.h"
#include "nanomsg/nn.h"
#include "nanomsg/pipeline.h"
#include "nanomsg/pubsub.h"
#include "nanomsg/tcp.h"

void *publisher(void *args)
{
    pthread_detach(pthread_self());
    srand(time(NULL));

    pubsub_info *info = (pubsub_info *) args;
    char *ip = info->broker_ip;
    int port = info->recv_port;
    int msg_size = info->msg_size;
    int num_channels = info->num_channels;
    char **channels = info->channels;

    char pub_addr[SIZE32];
    memset(pub_addr, '\0', SIZE32);
    sprintf(pub_addr, "tcp://%s:%d", ip, port);

    int rc, opt = 1;
    int pub_fd;
    pub_fd = nn_socket(AF_SP, NN_PUSH);
    assert(pub_fd != -1);
    rc = nn_setsockopt(pub_fd, NN_TCP, NN_TCP_NODELAY, &opt, sizeof(opt));
    assert(rc == 0);
    rc = nn_connect(pub_fd, pub_addr);

    char *msg = (char *) malloc(msg_size + SIZE8);
    char *msg_cont = (char *) malloc(msg_size + 1);
    memset(msg_cont, 'x', msg_size);
    msg_cont[msg_size] = '\0';

    while (1) {
        int channel = rand() % num_channels;
        int len = sprintf(msg, "%s %s", channels[channel], msg_cont);
        msg[len] = '\0';
        nn_send(pub_fd, msg, len + 1, 0);
    }

    free(msg_cont);
    free(msg);

    return NULL;
}

void *subscriber(void *args)
{
    pthread_detach(pthread_self());
    srand(time(NULL));

    pubsub_info *info = (pubsub_info *) args;
    char *ip = info->broker_ip;
    int sub_port = info->send_port;
    int report_port = info->recv_port;
    int num_channels = info->num_channels;
    char **channels = info->channels;
    int verbose = info->verbose;

    char sub_addr[SIZE32], report_addr[SIZE32];
    memset(sub_addr, '\0', SIZE32);
    memset(report_addr, '\0', SIZE32);
    sprintf(sub_addr, "tcp://%s:%d", ip, sub_port);
    sprintf(report_addr, "tcp://%s:%d", ip, report_port);

    int rc, opt = 1;
    int sub_fd, report_fd;

    sub_fd = nn_socket(AF_SP, NN_SUB);
    assert(sub_fd != -1);
    rc = nn_connect(sub_fd, sub_addr);
    assert(rc >= 0);

    report_fd = nn_socket(AF_SP, NN_PUSH);
    assert(report_fd != -1);
    rc = nn_setsockopt(report_fd, NN_TCP, NN_TCP_NODELAY, &opt, sizeof(opt));
    assert(rc == 0);
    rc = nn_connect(report_fd, report_addr);
    assert(rc >= 0);

    int i;
    for (i = 0; i < num_channels; i++) {
        rc = nn_setsockopt(sub_fd, NN_SUB, NN_SUB_SUBSCRIBE, channels[i], strlen(channels[i]));
        assert(rc >= 0);
    }

    int bytes = -1, messages = 0;
    INT64 now, last;
    get_timestamp(&last);
    while (1) {
        char *buf = NULL;
        bytes = nn_recv(sub_fd, &buf, NN_MSG, 0);
        assert (bytes >= 0);
        nn_freemsg (buf);

        messages++;
        get_timestamp(&now);
        if (now - last > 1000) {
            if (verbose) {
                printf("%d msg/sec\n", messages);
            }
            char *msg = (char *) malloc(SIZE16);
            int len = sprintf(msg, "%s %d", "metrics", messages);
            msg[len++] = '\0';
            bytes = nn_send(report_fd, msg, len, 0);
            assert(bytes == len);
            last = now;
            messages = 0;
            free(msg);
        }
    }
    nn_shutdown(sub_fd, 0);
    nn_shutdown(report_fd, 0);

    return NULL;
}

void usage()
{
    fprintf(stderr, "Usage: ./nanomsg_pubsub_cliet [OPTIONS]\n");
    fprintf(stderr, "  -b <broker_ip>       \tbroker ip(default 127.0.0.1)\n");
    fprintf(stderr, "  -r <receiver_port>   \treceiver port(default 5562)\n");
    fprintf(stderr, "  -s <sender_port>     \tsender port(default 5561)\n");
    fprintf(stderr, "  -t <run_seconds>     \tclient run time in seconds(default 10s)\n");
    fprintf(stderr, "  -S <message_size>    \tmessage in byte(default 20byte)\n");
    fprintf(stderr, "  -C <client_num>      \tnumber of pub/sub clients(default 1/2 cpu count)\n");
    fprintf(stderr, "  -v                   \tverbose mode\n");
    fprintf(stderr, "  -h                   \tOutput this help and exit.\n");
}

int main(int argc, char *argv[])
{
    char *broker_ip = strdup("127.0.0.1");
    int recv_port = 5562;
    int send_port = 5561;

    int run_seconds = 10;
    int num_channels = 50;
    int msg_size = 20;
    int num_clients = get_cpu_count() / 2;
    num_clients = num_clients > 0 ? num_clients : 1;

    int verbose = 0;

    int opt;
    while ((opt = getopt(argc, argv, "b:r:s:t:S:C:vh")) != -1) {
        switch (opt) {
        case 'b':
            free(broker_ip);
            broker_ip = strdup(optarg);
            break;
        case 'r':
            recv_port = atoi(optarg);
            break;
        case 's':
            send_port = atoi(optarg);
            break;
        case 't':
            run_seconds = atoi(optarg);
            break;
        case 'S':
            msg_size = atoi(optarg);
            break;
        case 'C':
            num_clients = atoi(optarg);
            break;
        case 'v':
            verbose = 1;
            break;
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
        default:
            fprintf(stderr, "Try 'nanomsg_pubsub_cliet -h' for more information");
            exit(EXIT_FAILURE);
        }
    }

    int i;
    char **channels = (char **) malloc(sizeof(char *) * num_channels);
    for (i = 0; i < num_channels; i++) {
        channels[i] = (char *) malloc(SIZE8);
        memset(channels[i], '\0', SIZE8);
        sprintf(channels[i], "%d", i);
    }
    pubsub_info *info = (pubsub_info *) malloc(sizeof(pubsub_info));
    info->broker_ip = broker_ip;
    info->recv_port = recv_port;
    info->send_port = send_port;
    info->msg_size = msg_size;
    info->num_channels = num_channels;
    info->channels = channels;
    info->verbose = verbose;

    pthread_t *pub_threads = (pthread_t *) malloc(sizeof(pthread_t) * num_clients);
    for (i = 0; i < num_clients; i++) {
        pthread_create(&pub_threads[i], NULL, publisher, (void *)info);
    }

    sleep(1);

    pthread_t *sub_threads = (pthread_t *) malloc(sizeof(pthread_t) * num_clients);
    for (i = 0; i < num_clients; i++) {
        pthread_create(&sub_threads[i], NULL, subscriber, (void *)info);
    }

    int rc;
    int metrics_fd;
    metrics_fd = nn_socket(AF_SP, NN_SUB);
    assert(metrics_fd != -1);
    char sub_addr[SIZE32];
    rc = sprintf(sub_addr, "tcp://%s:%d", broker_ip, send_port);
    sub_addr[rc] = '\0';
    rc = nn_connect(metrics_fd, sub_addr);
    assert(rc >= 0);
    rc = nn_setsockopt(metrics_fd, NN_SUB, NN_SUB_SUBSCRIBE, "metrics", 7);
    assert(rc == 0);

    INT64 start, now;
    get_timestamp(&start);
    int *stats = (int *) malloc(sizeof(int) * num_clients * run_seconds);
    int cnt = 0;
    while (1) {
        get_timestamp(&now);
        if (now - start > run_seconds * 1000) {
            break;
        }
        char *msg = NULL;

        int bytes = nn_recv(metrics_fd, &msg, NN_MSG, 0);
        assert (bytes >= 0);
        char *start = strchr(msg, ' ') + 1;
        stats[cnt++] = atoi(start);

        nn_freemsg (msg);
    }
    printf("%d median msg/sec\n", get_median(stats, cnt));

    free(broker_ip);
    free(info);
    free(stats);
    printf("done!\n");
    return 0;
}
