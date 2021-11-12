#ifdef WLIS_H
#define WLIS_H

#include <string>

class WLIS {
    void init(std::string inputFileName);
    void run(std::string inputFileName, std::string outputFileName, std::string algoType);
}


#endif