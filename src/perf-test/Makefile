WARN=-Wall
CFLAGS=$(STD) $(WARN)

all: zmq_req_perf nanomsg_req_perf \
	zmq_req_srv_perf zmq_req_cli_perf \
	nanomsg_req_srv_perf nanomsg_req_cli_perf \
	zmq_pubsub_broker zmq_pubsub_client
.PHONY: all

zmq_req_perf: zmq_req_perf.o util.o
	$(CC) $(CFLAGS) -o $@ $^ -lzmq -lczmq

nanomsg_req_perf: nanomsg_req_perf.o util.o
	$(CC) $(CFLAGS) -o $@ $^ -lnanomsg -lczmq

zmq_req_srv_perf: zmq_req_srv_perf.o
	$(CC) $(CFLAGS) -o $@ $^ -lzmq -lczmq

zmq_req_cli_perf: zmq_req_cli_perf.o util.o
	$(CC) $(CFLAGS) -o $@ $^ -lzmq -lczmq

nanomsg_req_srv_perf: nanomsg_req_srv_perf.o
	$(CC) $(CFLAGS) -o $@ $^ -lnanomsg -lczmq

nanomsg_req_cli_perf: nanomsg_req_cli_perf.o util.o
	$(CC) $(CFLAGS) -o $@ $^ -lnanomsg -lczmq

zmq_pubsub_broker: zmq_pubsub_broker.o util.o
	$(CC) $(CFLAGS) -o $@ $^ -lpthread -lczmq

zmq_pubsub_client: zmq_pubsub_client.o util.o
	$(CC) $(CFLAGS) -o $@ $^ -lpthread -lczmq

clean:
	rm -rf *.o
	rm -rf zmq_req_perf nanomsg_req_perf \
		zmq_req_srv_perf zmq_req_cli_perf \
		nanomsg_req_srv_perf nanomsg_req_cli_perf \
		zmq_pubsub_broker zmq_pubsub_client
.PHONY: clean

