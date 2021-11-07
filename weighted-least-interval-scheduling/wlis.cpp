#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include "wlis.h"
#include <algorithm>
#include <map>
#include "omp.h"

class Interval {
public:
    int weight;
    int start;
    int finish;
    int id;

    Interval(int start, int finish, int weight) : 
        start(start), finish(finish), weight(weight) {
        this->id = -1;
    }

    bool operator<(Interval const &other) {
        return this->finish < other.finish;
    }

    std::string toString() {
        if (this->id != -1) {
            return "id: (" + std::to_string(id) + ") weight: (" + std::to_string(weight) + ") start: (" + std::to_string(start) + ") finish: (" + std::to_string(finish) + ")";
        } else {
            return "weight: (" + std::to_string(weight) + ") start: (" + std::to_string(start) + ") finish: (" + std::to_string(finish) + ")";
        }
    }

    void print() {
        std::cout << this->toString() << std::endl;
    }

    void setId(int id) {
        this->id = id;
    }
};

namespace {
    Interval parseInterval(std::string s) {
        // {start} {end} {weight}
        std::string delimiter = " ";

        size_t last = 0;
        size_t next = 0;
        std::string token;
        std::vector<std::string> tokens;
        while ((next = s.find(delimiter, last)) != std::string::npos) {
            token = s.substr(last, next-last);
            tokens.push_back(token);
            last = next + 1;
        }

        Interval i(std::stoi(tokens.at(0)), std::stoi(tokens.at(1)), std::stoi(s.substr(last)));
        return i;
    }
}

class WLIS {
    
    std::vector<Interval> inputIntervals;
    std::vector<Interval> soln;

    void parseInput(std::string inputFileName) {
        std::cout << inputFileName << std::endl;
        std::ifstream infile;
        infile.open(inputFileName, std::ifstream::in);

        std::string line;
        if (infile.is_open()) {
            while (getline(infile, line)) {
                Interval i = parseInterval(line);
                inputIntervals.push_back(i);
            }
            infile.close();
        } else {
            // input file name is invalid or does not exist
            std::cout << "INVALID_FILE_NAME" << std::endl;
        }
    }

    int getP(std::vector<Interval> intervals, Interval interval) {
        // Use binary search to find the correct interval
        Interval compatible(0, interval.start+1, 0);
        auto lower = std::lower_bound(intervals.begin(), intervals.end(), compatible);
        if (lower == intervals.begin()) {
            return -1;
        } else {
            lower--;
            return (*lower).id;
        }
    }

    std::vector<Interval> wlisSeq() {
        // First, sort the intervals in ascending order of their finish times
        std::vector<Interval> intervals = this->inputIntervals;
        int numIntervals = intervals.size();
        std::sort (intervals.begin(), intervals.end());
        for (int i = 0; i < numIntervals; i++) {
            intervals.at(i).setId(i);
        }

        // Next, calculate P(j), where P(j) is the rightmost interval i such that i and j are compatible (f_i <= s_j)
        int P [numIntervals];
        for (auto &interval : intervals) {
            P[interval.id] = getP(intervals, interval);
        }

        // Then, use the recurrence equation: OPT(j) = max(weight(j) + OPT(P[j]), OPT(j-1))
        std::map<int, int> OPT { {-1, 0}, {0, 0}, };
        std::map<int, std::vector<Interval>> OPT_path;

        for (int i = 0; i < numIntervals; i++) {
            Interval currentInterval = intervals.at(i);
            int with = currentInterval.weight + OPT[P[i]];
            int without = OPT[i-1];
            if (with > without) {
                // choose the current one
                OPT_path[i] = OPT_path[P[i]];
                OPT_path[i].push_back(currentInterval);
                OPT[i] = with;
            } else {
                OPT_path[i] = OPT_path[i-1];
                OPT[i] = without;
            }
        }

        return OPT_path[numIntervals-1];
    }

    std::vector<Interval> wlisParBF() {
        std::vector<Interval> soln;
        return soln;
    }

    std::vector<Interval> wlisParDP() {
        // Set num threads
        bool debug = false;
        int par_enabled = 1;
        omp_set_num_threads(4);

        // First, sort the intervals in ascending order of their finish times
        std::vector<Interval> intervals = this->inputIntervals;
        int numIntervals = intervals.size();
        std::sort (intervals.begin(), intervals.end());
        #pragma omp parallel for if(par_enabled)
        for (int i = 0; i < numIntervals; i++) {
            intervals.at(i).setId(i);
        }

        // Next, calculate P(j), where P(j) is the rightmost interval i such that i and j are compatible (f_i <= s_j)
        // We can do this in parallel
        int P [numIntervals];
        #pragma omp parallel for if(par_enabled)
        for (int i = 0; i < numIntervals; i++) {
            Interval interval = intervals.at(i);
            P[i] = getP(intervals, interval);
        }

        // Then, we use an LLP algorithm to find the global state G
        int G [numIntervals+1];
        int GPrev [numIntervals+1];
        bool diffs [numIntervals];

        G[0] = 0;
        GPrev[0] = 0;

        #pragma omp parallel for if(par_enabled)
        for (int i = 0; i < numIntervals; i++) {
            G[i+1] = 0;
            GPrev[i+1] = -1;
            diffs[i] = true;
        }

        bool diffExists = true;
        int count = 0;

        while (diffExists) {
            if (debug) {
                std::cout << "----------------------------------------" << std::endl;
                std::cout << "DEBUG:" << std::endl;
                std::cout << "Diffs Array:" << std::endl;
                for (int i = 0; i < numIntervals; i++) {
                    if (diffs[i]) {
                        std::cout << "T "; 
                    } else {
                        std::cout << "F ";
                    }
                }
                std::cout << std::endl;

                std::cout << "G Array:" << std::endl;
                for (int i = 0; i < numIntervals+1; i++) {
                    std::cout << i << " ";
                }
                std::cout << std::endl;
                for (int i = 0; i < numIntervals+1; i++) {
                    std::cout << G[i] << " ";
                }
                std::cout << std::endl;

                std::cout << "GPrev Array:" << std::endl;
                for (int i = 0; i < numIntervals+1; i++) {
                    std::cout << i << " ";
                }
                std::cout << std::endl;
                for (int i = 0; i < numIntervals+1; i++) {
                    std::cout << GPrev[i] << " ";
                }
                std::cout << std::endl;
            }
            
            int GCalc [numIntervals];
            #pragma omp parallel if(par_enabled)
            {
                #pragma omp for
                for (int i = 0; i < numIntervals; i++) {
                    if (G[i+1] != GPrev[i+1]) {
                        // update 
                        if (debug)
                            std::cout << std::to_string(intervals.at(i).weight) << " " << std::to_string(P[i]+1) << " " << std::to_string(G[P[i]+1]) << " | " << G[i] << std::endl;
                        GCalc[i] = std::max(intervals.at(i).weight + G[P[i]+1], G[i]);
                        diffs[i] = true;
                    } else {
                        diffs[i] = false;
                    }
                }
                
                #pragma omp for
                for (int i = 0; i < numIntervals; i++) {
                    GPrev[i+1] = G[i+1];
                    G[i+1] = GCalc[i];
                }
            }


            diffExists = false;
            #pragma omp parallel for reduction(|:diffExists)
            for (int i = 0; i < numIntervals; i++) {
                diffExists |= diffs[i];
            }
            count++;

            if (debug) {
                std::cout << "G Array:" << std::endl;
                for (int i = 0; i < numIntervals+1; i++) {
                    std::cout << i << " ";
                }
                std::cout << std::endl;
                for (int i = 0; i < numIntervals+1; i++) {
                    std::cout << G[i] << " ";
                }
                std::cout << std::endl;

                std::cout << "GPrev Array:" << std::endl;
                for (int i = 0; i < numIntervals+1; i++) {
                    std::cout << i << " ";
                }
                std::cout << std::endl;
                for (int i = 0; i < numIntervals+1; i++) {
                    std::cout << GPrev[i] << " ";
                }
                std::cout << std::endl;

                std::cout << "END DEBUG" << std::endl;
                std::cout << "----------------------------------------" << std::endl;
            }
        }

        for (int i = 0; i < numIntervals+1; i++) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
        for (int i = 0 ; i < numIntervals+1; i++) {
            std::cout << G[i] << " ";
        }
        std::cout << std::endl;

        if (debug) std::cout << "count: " << std::to_string(count) << std::endl;

        std::vector<Interval> soln;
        return soln;
    }

public:

    void init(std::string inputFileName) {
        parseInput(inputFileName);
    }

    std::vector<Interval> run(std::string algoType) {
        std::vector<Interval> soln;
        if (algoType == "seq") {
            soln = wlisSeq();
        } else if (algoType == "parbf") {
            soln = wlisParBF();
        } else if (algoType == "pardp") {
            soln = wlisParDP();
        } else {
            // Invalid algoType, print error
        }
        return soln;
    }

};

int main(int argc, char *argv[]) {

    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    WLIS wlis;
    wlis.init(argv[1]);

    auto t1 = high_resolution_clock::now();
    // std::vector<Interval> soln = wlis.run("seq");
    auto t2 = high_resolution_clock::now();
    
    // std::cout << "Sequential DP Solution:" << std::endl;
    // for (int i = 0; i < soln.size(); i++) {
    //     std::cout << soln.at(i).toString() << std::endl;
    // }

    auto t3 = high_resolution_clock::now();
    // wlis.run("parbf");
    auto t4 = high_resolution_clock::now();

    auto t5 = high_resolution_clock::now();
    wlis.run("pardp");
    auto t6 = high_resolution_clock::now();

    /* Getting number of milliseconds as an integer. */
    duration<double, std::milli> ms_double_seq = t2 - t1;
    duration<double, std::milli> ms_double_par_bf = t4 - t3;
    duration<double, std::milli> ms_double_par_dp = t6 - t5;


    std::cout << ms_double_seq.count() << "ms\n";
    std::cout << ms_double_par_bf.count() << "ms\n";
    std::cout << ms_double_par_dp.count() << "ms\n";

    return 0;
}
