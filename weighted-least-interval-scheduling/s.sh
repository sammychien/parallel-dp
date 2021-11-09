#!/bin/bash

g++ wlis.cpp -fopenmp -o main.out

./main.out ./tests/100.txt
