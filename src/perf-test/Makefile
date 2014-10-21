BUILD_PATH = .
SRC_PATH = .
SRC_EXT = c
INCLUDES = -I $(SRC_PATH)/
WARN = -Wall
CFLAGS = $(STD) $(WARN)
DCOMPILE_FLAGS = -g
COMPILE_FLAGS = -O3
release: CFLAGS += $(COMPILE_FLAGS)
debug: CFLAGS += $(DCOMPILE_FLAGS)

SOURCES = $(shell find $(SRC_PATH)/ -name '*.$(SRC_EXT)')
OBJECTS = $(SOURCES:$(SRC_PATH)/%.$(SRC_EXT)=$(BUILD_PATH)/%.o)

all: zmq_req_perf nanomsg_req_perf \
	zmq_req_srv_perf zmq_req_cli_perf \
	nanomsg_req_srv_perf nanomsg_req_cli_perf \
	zmq_pubsub_broker zmq_pubsub_client \
	nanomsg_pubsub_broker nanomsg_pubsub_client
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
	$(CC) $(CFLAGS) -o $@ $^ -lnanomsg

nanomsg_req_cli_perf: nanomsg_req_cli_perf.o util.o
	$(CC) $(CFLAGS) -o $@ $^ -lnanomsg -lczmq

zmq_pubsub_broker: zmq_pubsub_broker.o util.o
	$(CC) $(CFLAGS) -o $@ $^ -lpthread -lczmq

zmq_pubsub_client: zmq_pubsub_client.o util.o
	$(CC) $(CFLAGS) -o $@ $^ -lpthread -lczmq

nanomsg_pubsub_broker: nanomsg_pubsub_broker.o util.o
	$(CC) $(CFLAGS) -o $@ $^ -lnanomsg

nanomsg_pubsub_client: nanomsg_pubsub_client.o util.o
	$(CC) $(CFLAGS) -o $@ $^ -lnanomsg

$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(SRC_EXT)
	@echo "Compiling: $< -> $@"
	$(CC) $(CFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@

.PHONY: release
release: $(OBJECTS)
	@echo "Beginning release build"
	@$(MAKE) all --no-print-directory

.PHONY: debug
debug: $(OBJECTS)
	@echo "Beginning debug build"
	@$(MAKE) all --no-print-directory

clean:
	rm -rf *.o
	rm -rf *.d
	rm -rf zmq_req_perf nanomsg_req_perf \
		zmq_req_srv_perf zmq_req_cli_perf \
		nanomsg_req_srv_perf nanomsg_req_cli_perf \
		zmq_pubsub_broker zmq_pubsub_client \
		nanomsg_pubsub_broker nanomsg_pubsub_client
.PHONY: clean

