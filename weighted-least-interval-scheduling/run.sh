#!/bin/bash

for filename in tests/*.txt; do
	./main.out "$filename" >> res.csv
done
