WARN=-Wall

CFLAGS=$(STD) $(WARN)
FINAL_LIBS=-lm -lnanomsg

PIPELINE_NAME=pipeline
PIPELINE_OBJ=pipeline.o
REQREP_NAME=reqrep
REQREP_OBJ=reqrep.o
PAIR_NAME=pair
PAIR_OBJ=pair.o
PUBSUB_NAME=pubsub
PUBSUB_OBJ=pubsub.o
SURVEY_NAME=survey
SURVEY_OBJ=survey.o
BUS_NAME=bus
BUS_OBJ=bus.o

all: $(PIPELINE_NAME) $(REQREP_NAME) $(PAIR_NAME) $(PUBSUB_NAME) $(SURVEY_NAME) $(BUS_NAME)
.PHONY: all

$(PIPELINE_NAME): $(PIPELINE_OBJ)
	$(CC) -o $@ $^ $(FINAL_LIBS)

$(REQREP_NAME): $(REQREP_OBJ)
	$(CC) -o $@ $^ $(FINAL_LIBS)

$(PAIR_NAME): $(PAIR_OBJ)
	$(CC) -o $@ $^ $(FINAL_LIBS)

$(PUBSUB_NAME): $(PUBSUB_OBJ)
	$(CC) -o $@ $^ $(FINAL_LIBS)

$(SURVEY_NAME): $(SURVEY_OBJ)
	$(CC) -o $@ $^ $(FINAL_LIBS)

$(BUS_NAME): $(BUS_OBJ)
	$(CC) -o $@ $^ $(FINAL_LIBS)

test:
	@(cd ../tests; ./runtest)

clean:
	rm *.o
	rm -rf $(PIPELINE_NAME) $(REQREP_NAME) $(PAIR_NAME) $(PUBSUB_NAME) $(SURVEY_NAME) $(BUS_NAME)
.PHONY: clean
