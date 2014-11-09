#!/usr/bin/env python

import sys
from os import mkdir, chdir
from os.path import join
from multiprocessing import cpu_count
from subprocess import check_output, PIPE, Popen
from getopt import getopt

output_path = lambda s="": join("output", s)


# All test_client runs and their cli args.
runs = {
    "nanomsg_pubsub": ["./build/nanomsg_pubsub_client", "-r5000", "-s5001"],
    "zeromq_pubsub": ["./build/zmq_pubsub_client", "-r6000", "-s6001"],
}


# Consistent graph colours defined for each of the runs.
colours = {
    "nanomsg_pubsub": "red",
    "zeromq_pubsub": "green",
    # "blue", "violet", "orange"
}


# Groups of runs mapped to each graph.
plots = {
    "0MQ-vs-Nanomsg": ["zeromq_pubsub", "nanomsg_pubsub"],
}


def run_clients(args):
    """
    Runs the test_client program for 0MQ or Nanomsg, for the range
    from 1 to cpus * 2 as the number of clients, returning the
    median messsages per second for each.
    """
    results = []
    num_runs = cpu_count() * 2
    for clients in range(1, num_runs + 1):
        bar = ("#" * clients).ljust(num_runs)
        sys.stdout.write("\r[%s] %s/%s " % (bar, clients, num_runs))
        sys.stdout.flush()

        run_args = args[:]
        run_args.extend(["-C%s" % clients])
        print run_args
        out = check_output(run_args, stderr=PIPE)

        results.append(out.split(" ")[0].strip())

    sys.stdout.write("\n")
    return results


def prepare():
    # Store all results in an output directory.
    try:
        mkdir(output_path())
    except OSError, e:
        print e


def bench():
    prepare()
    # Store results from each test_client run into files.
    for name, args in runs.iteritems():
        with open(output_path(name + ".dat"), "w") as f:
            f.write("\n".join(run_clients(args)))


def draw():
    with open("plot.p", "r") as f:
        plotfile = f.read()

    # change working dir to output
    chdir(output_path())

    line = '"%s.dat" using ($0+1):1 with lines title "%s" lw 2 lt rgb "%s"'
    for name, names in plots.items():
        #name = output_path(name)
        with open(names[0] + ".dat", "r") as f:
            clients = len(f.read().split())
        with open(name + ".p", "w") as f:
            lines = ", ".join([line % (l, l.replace("_", " "), colours[l])
                            for l in names])
            f.write(plotfile % {"name": name, "lines": lines, "clients": clients})
        Popen(["gnuplot", name + ".p"], stderr=sys.stdout)


def show_help():
    print '''usage: bench.py [-h] [--draw-only] [--run-only]

optional arguments:
  -h, --help            show this help message and exit
  --draw-only           generate graphs only without running benchmark
  --run-only            run benchmark only

Online help: <https://github.com/amyangfei/nanomsg-examples>
'''


if __name__ == '__main__':
    do_bench = True
    do_draw = True
    shortopts = 'h'
    longopts = ['draw-only', 'run-only']
    optlist, args = getopt(sys.argv[1:], shortopts, longopts)
    for key, val in optlist:
        if key == '--draw-only':
            do_bench = False
        elif key == '--run-only':
            do_draw = False
        elif key == '-h':
            show_help()
            sys.exit(0)
    if do_bench:
        bench()
    if do_draw:
        draw()

