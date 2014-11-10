#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "czmq.h"

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

    zctx_t *ctx = zctx_new();
    void *pub = zsocket_new(ctx, ZMQ_PUSH);
    zsocket_connect(pub, "tcp://%s:%d", ip, port);

    char *msg_cont = (char *) malloc(msg_size + 1);
    memset(msg_cont, 'x', msg_size);
    msg_cont[msg_size] = '\0';
    char *msg = (char *) malloc(msg_size + SIZE8);

    while (1) {
        int channel = rand() % num_channels;
        int len = sprintf(msg, "%s %s", channels[channel], msg_cont);
        msg[len] = '\0';
        zstr_send(pub, msg);
    }

    free(msg_cont);
    free(msg);
    zctx_destroy(&ctx);

    return NULL;
}

void *subscriber(void *args)
{
    pthread_detach(pthread_self());
    srand(time(NULL));

    pubsub_info *info = (pubsub_info *) args;
    char *ip = info->broker_ip;
    int port = info->send_port;
    int report_port = info->recv_port;
    int num_channels = info->num_channels;
    char **channels = info->channels;
    int verbose = info->verbose;

    zctx_t *ctx = zctx_new();
    void *sub = zsocket_new(ctx, ZMQ_SUB);
    zsocket_connect(sub, "tcp://%s:%d", ip, port);
    void *reporter = zsocket_new(ctx, ZMQ_PUSH);
    zsocket_connect(reporter, "tcp://%s:%d", ip, report_port);

    int i;
    for (i = 0; i < num_channels; i++) {
        zsocket_set_subscribe(sub, channels[i]);
    }

    int messages = 0;
    INT64 now, last;
    get_timestamp(&last);
    while (1) {
        char *msg = zstr_recv(sub);
        if (!msg) {
            break; //interrupt
        }
        messages++;
        get_timestamp(&now);
        if (now - last > 1000) {
            if (verbose) {
                printf("%d msg/sec\n", messages);
            }
            zstr_sendf(reporter, "%s %d", "metrics", messages);
            last = now;
            messages = 0;
        }
        free(msg);
    }

    zctx_destroy(&ctx);

    return NULL;
}

void usage()
{
    fprintf(stderr, "Usage: ./zmq_pubsub_client [OPTIONS]\n");
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
            fprintf(stderr, "Try 'zmq_pubsub_broker -h' for more information");
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
        /*zthread_new(publisher, (void *)info);*/
    }

    sleep(1);

    pthread_t *sub_threads = (pthread_t *) malloc(sizeof(pthread_t) * num_clients);
    for (i = 0; i < num_clients; i++) {
        pthread_create(&sub_threads[i], NULL, subscriber, (void *)info);
        /*zthread_new(subscriber, (void *)info);*/
    }

    zctx_t *ctx = zctx_new();
    void *sub = zsocket_new(ctx, ZMQ_SUB);
    zsocket_connect(sub, "tcp://%s:%d", broker_ip, send_port);
    zsocket_set_subscribe(sub, "metrics");

    INT64 start, now;
    get_timestamp(&start);
    int *stats = (int *) malloc(sizeof(int) * num_clients * run_seconds);
    int cnt = 0;
    while (1) {
        get_timestamp(&now);
        if (now - start > run_seconds * 1000) {
            break;
        }
        char *msg = zstr_recv(sub);
        if (!msg) {
            break; //interrupt
        } else {
            char *start = strchr(msg, ' ') + 1;
            stats[cnt++] = atoi(start);
            free(msg);
        }
    }
    printf("%d median msg/sec\n", get_median(stats, cnt));
    zctx_destroy(&ctx);

    free(broker_ip);
    free(info);
    free(stats);
    printf("done!\n");
    return 0;
}
