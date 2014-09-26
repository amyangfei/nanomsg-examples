WARN=-Wall
CFLAGS=$(STD) $(WARN)

all: zmq_req_perf nanomsg_req_perf
.PHONY: all

zmq_req_perf: zmq_req_perf.o util.o
	$(CC) $(CFLAGS) -o $@ $^ -lzmq -lczmq

nanomsg_req_perf: nanomsg_req_perf.o util.o
	$(CC) $(CFLAGS) -o $@ $^ -lnanomsg -lczmq

clean:
	rm -rf *.o
	rm -rf zmq_req_perf nanomsg_req_perf
.PHONY: clean

