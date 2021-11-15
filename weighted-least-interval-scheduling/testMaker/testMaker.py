import random
from math import floor


def main():
    for inputSize in range(500, 1001):
        filePath = "../tests/{inputSize}.txt".format(inputSize = inputSize)
        f = open(filePath, "w")

        for i in range(inputSize):
            start = random.randint(0, inputSize)
            stop = start + random.randint(1, floor(inputSize/2))
            weight = random.randint(1, floor(inputSize/4))
            string = "{start} {stop} {weight}\n".format(start = start, stop = stop, weight = weight) 
            f.write(string)
        
        f.close()

if __name__ == "__main__":
    main()
