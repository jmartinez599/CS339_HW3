# Cache Simulator (CS339 - HW3)

## Overview
This project implements a cache simulator in C++. The simulator models the behavior of a set associative cache and determine whether each memory reference results in a hit or miss.

Simulator takes as input:
- Total number of cache entries
- Cache associativity
- A file containing memory address refrences

The output is written to a file named:
cache_sim_output

Each line in the output corresponds to one memory reference in the input file.

## Compilation Instructions
use g++ to compile program:
g++ -o cache_sim cache_sim.cpp

## How to run
Run the program using:
./cache_sim <num_entries><associativity><input_file>

