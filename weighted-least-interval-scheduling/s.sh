#!/bin/bash

g++ wlis.cpp -fopenmp -o main.out

./main.out ./tests/10.txt ./tests/100.txt ./tests/1000.txt ./tests/10000.txt ./tests/100000.txt
