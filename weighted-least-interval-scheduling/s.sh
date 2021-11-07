#!/bin/bash

g++ wlis.cpp -fopenmp -o main.out

./main.out ./tests/1.txt
