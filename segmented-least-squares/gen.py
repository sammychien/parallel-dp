#! /usr/bin/python3
import random
import os
import shutil
import subprocess
import time
import zipfile

random.seed(19832)
tc = 0

def writeTestCase(test_string):
    global tc
    tc += 1
    print(tc)
    filename = 'input/input%02d.txt' % tc
    with open(filename, 'w') as fout:
        print(test_string, file=fout)

def prep():
    inpath = os.getcwd() + '/input'
    outpath = os.getcwd() + '/output'
    shutil.rmtree(inpath, True)
    os.mkdir(inpath)
    shutil.rmtree(outpath, True)
    os.mkdir(outpath)
    time.sleep(1)


def randGen(size):
    result = ""
    n = size
    c = random.randint(0, 100)
    result += str(n) + " " + str(c) + "\n"
    for i in range(n):
        result += str(i) + " " + str(random.randint(0, 3000)) + "\n"
    return result

def main():
    global tc
    prep()
    for sz in range(10, 1000):
        for j in range(2):
            writeTestCase(randGen(sz))



if __name__ == '__main__':
    main()