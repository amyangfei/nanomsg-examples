#!/bin/bash

src_base=../src
pipeline=$src_base/pipeline
reqrep=$src_base/reqrep
pair=$src_base/pair
pubsub=$src_base/pubsub
survey=$src_base/survey
bus=$src_base/bus
sep='------------------------------'

pipline_test() {
    echo "starting test pipeline..."
    $pipeline node0 ipc:///tmp/pipeline.ipc & node0=$!
    $pipeline node1 ipc:///tmp/pipeline.ipc "Hello, World!"
    $pipeline node1 ipc:///tmp/pipeline.ipc "Goodbye."
    kill $node0
    wait $node0 2>/dev/null
    echo "end test pipeline..."
    echo $sep
}

reqrep_test() {
    echo "starting test reqrep..."
    $reqrep node0 ipc:///tmp/reqrep.ipc & node0=$!
    $reqrep node1 ipc:///tmp/reqrep.ipc
    kill $node0
    wait $node0 2>/dev/null
    echo "end test reqrep..."
    echo $sep
}

pair_test() {
    echo "starting test pair..."
    $pair node0 ipc:///tmp/pair.ipc & node0=$!
    $pair node1 ipc:///tmp/pair.ipc & node1=$!
    sleep 3
    kill $node0 $node1
    wait $node0 $node1 2>/dev/null
    echo "end test pair..."
    echo $sep
}

pubsub_test() {
    echo "starting test pubsub..."
    $pubsub server ipc:///tmp/pubsub.ipc & server=$!
    $pubsub client ipc:///tmp/pubsub.ipc client0 & client0=$!
    $pubsub client ipc:///tmp/pubsub.ipc client1 & client1=$!
    $pubsub client ipc:///tmp/pubsub.ipc client2 & client2=$!
    sleep 3
    kill $server $client0 $client1 $client2
    wait $server $client0 $client1 $client2 2>/dev/null
    echo "end test pubsub..."
    echo $sep
}

survey_test() {
    echo "starting test survey..."
    $survey server ipc:///tmp/survey.ipc & server=$!
    $survey client ipc:///tmp/survey.ipc client0 & client0=$!
    $survey client ipc:///tmp/survey.ipc client1 & client1=$!
    $survey client ipc:///tmp/survey.ipc client2 & client2=$!
    sleep 1
    kill $server $client0 $client1 $client2
    wait $server $client0 $client1 $client2 2>/dev/null
    echo "end test survey..."
    echo $sep
}

bus_test() {
    echo "starting test bus..."
    $bus node0 ipc:///tmp/node0.ipc ipc:///tmp/node1.ipc ipc:///tmp/node2.ipc & node0=$!
    $bus node1 ipc:///tmp/node1.ipc ipc:///tmp/node2.ipc ipc:///tmp/node3.ipc & node1=$!
    $bus node2 ipc:///tmp/node2.ipc ipc:///tmp/node3.ipc & node2=$!
    $bus node3 ipc:///tmp/node3.ipc ipc:///tmp/node0.ipc & node3=$!
    sleep 2
    kill $node0 $node1 $node2 $node3
    wait $node0 $node1 $node2 $node3 2>/dev/null
    echo "end test bus..."
    echo $sep
}

echo $sep
pipline_test $*
reqrep_test $*
pair_test $*
pubsub_test $*
survey_test $*
bus_test $*
