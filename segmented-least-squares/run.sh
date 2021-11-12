#!/bin/bash
for filename in input/*.txt; do
	./a.out < "$filename" >> res.csv
done
