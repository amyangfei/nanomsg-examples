BUILD_PATH = build
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

all: dirs $(BUILD_PATH)/zmq_req_perf $(BUILD_PATH)/nanomsg_req_perf \
	$(BUILD_PATH)/zmq_req_srv_perf $(BUILD_PATH)/zmq_req_cli_perf \
	$(BUILD_PATH)/nanomsg_req_srv_perf $(BUILD_PATH)/nanomsg_req_cli_perf \
	$(BUILD_PATH)/zmq_pubsub_broker $(BUILD_PATH)/zmq_pubsub_client \
	$(BUILD_PATH)/nanomsg_pubsub_broker $(BUILD_PATH)/nanomsg_pubsub_client
.PHONY: all

$(BUILD_PATH)/zmq_req_perf: $(BUILD_PATH)/zmq_req_perf.o $(BUILD_PATH)/util.o
	$(CC) $(CFLAGS) -o $@ $^ -lzmq -lczmq

$(BUILD_PATH)/nanomsg_req_perf: $(BUILD_PATH)/nanomsg_req_perf.o $(BUILD_PATH)/util.o
	$(CC) $(CFLAGS) -o $@ $^ -lnanomsg -lczmq

$(BUILD_PATH)/zmq_req_srv_perf: $(BUILD_PATH)/zmq_req_srv_perf.o
	$(CC) $(CFLAGS) -o $@ $^ -lzmq -lczmq

$(BUILD_PATH)/zmq_req_cli_perf: $(BUILD_PATH)/zmq_req_cli_perf.o $(BUILD_PATH)/util.o
	$(CC) $(CFLAGS) -o $@ $^ -lzmq -lczmq

$(BUILD_PATH)/nanomsg_req_srv_perf: $(BUILD_PATH)/nanomsg_req_srv_perf.o
	$(CC) $(CFLAGS) -o $@ $^ -lnanomsg

$(BUILD_PATH)/nanomsg_req_cli_perf: $(BUILD_PATH)/nanomsg_req_cli_perf.o $(BUILD_PATH)/util.o
	$(CC) $(CFLAGS) -o $@ $^ -lnanomsg -lczmq

$(BUILD_PATH)/zmq_pubsub_broker: $(BUILD_PATH)/zmq_pubsub_broker.o $(BUILD_PATH)/util.o
	$(CC) $(CFLAGS) -o $@ $^ -lpthread -lczmq

$(BUILD_PATH)/zmq_pubsub_client: $(BUILD_PATH)/zmq_pubsub_client.o $(BUILD_PATH)/util.o
	$(CC) $(CFLAGS) -o $@ $^ -lpthread -lczmq

$(BUILD_PATH)/nanomsg_pubsub_broker: $(BUILD_PATH)/nanomsg_pubsub_broker.o $(BUILD_PATH)/util.o
	$(CC) $(CFLAGS) -o $@ $^ -lnanomsg

$(BUILD_PATH)/nanomsg_pubsub_client: $(BUILD_PATH)/nanomsg_pubsub_client.o $(BUILD_PATH)/util.o
	$(CC) $(CFLAGS) -o $@ $^ -lpthread -lnanomsg

$(BUILD_PATH)/%.o: $(SRC_PATH)/%.$(SRC_EXT)
	@echo "Compiling: $< -> $@"
	$(CC) $(CFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@

.PHONY: release
release: dirs $(OBJECTS)
	@echo "Beginning release build"
	@$(MAKE) all --no-print-directory

.PHONY: debug
debug: dirs $(OBJECTS)
	@echo "Beginning debug build"
	@$(MAKE) all --no-print-directory

.PHONY: dirs
dirs:
	@echo "Creating directories"
	@mkdir -p $(dir $(OBJECTS))

clean:
	rm -rf build
.PHONY: clean

