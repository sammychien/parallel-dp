#!/bin/bash

g++ wlis.cpp -fopenmp -o main.out

export OMP_NUM_THREADS=4

(./main.out ./tests/10.txt ./tests/100.txt) > results.log
