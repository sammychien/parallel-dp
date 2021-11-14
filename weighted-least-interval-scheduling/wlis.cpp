#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include "wlis.h"
#include <algorithm>
#include <unordered_map>
#include "omp.h"

class Interval {
public:
    int start;
    int finish;
    int weight;
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
    bool sorted = false;

    void parseInput(std::string inputFileName) {
        std::vector<Interval>().swap(this->inputIntervals); // Deallocate and clear vector
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

    int getP(std::vector<Interval> &intervals, Interval interval) {
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

    int wlisSeq() {
        int numIntervals = this->inputIntervals.size();

        // First, sort the intervals in ascending order of their finish times if not already sorted
        if (!sorted) {
            std::sort (this->inputIntervals.begin(), this->inputIntervals.end());
            for (int i = 0; i < numIntervals; i++) {
                this->inputIntervals.at(i).setId(i);
            }
        }

        // Next, calculate P(j), where P(j) is the rightmost interval i such that i and j are compatible (f_i <= s_j)
        int P [numIntervals];
        for (auto &interval : this->inputIntervals) {
            P[interval.id] = getP(this->inputIntervals, interval);
        }

        // Then, use the recurrence equation: OPT(j) = max(weight(j) + OPT(P[j]), OPT(j-1))
        std::unordered_map<int, int> OPT { {-1, 0}, {0, 0}, };
        std::unordered_map<int, std::vector<Interval>> OPT_path;

        for (int i = 0; i < numIntervals; i++) {
            Interval currentInterval = this->inputIntervals.at(i);
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

        return OPT[numIntervals-1];
    }

    int wlisPar() {
        // Settings
        int par_enabled = 1;

        int numIntervals = this->inputIntervals.size();

        // First, sort the intervals in ascending order of their finish times if not already sorted
        if (!sorted) {
            std::sort (this->inputIntervals.begin(), this->inputIntervals.end());
            #pragma omp parallel for if(par_enabled)
            for (int i = 0; i < numIntervals; i++) {
                this->inputIntervals.at(i).setId(i);
            }
        }

        // Next, calculate P(j), where P(j) is the rightmost interval i such that i and j are compatible (f_i <= s_j)
        // We can do this in parallel
        int P [numIntervals];
        #pragma omp parallel for if(par_enabled)
        for (int i = 0; i < numIntervals; i++) {
            Interval interval = this->inputIntervals.at(i);
            P[i] = getP(this->inputIntervals, interval);
        }

        // Then, use the recurrence equation: OPT(j) = max(weight(j) + OPT(P[j]), OPT(j-1))
        std::unordered_map<int, int> OPT { {-1, 0}, {0, 0}, };
        std::unordered_map<int, std::vector<Interval>> OPT_path;

        for (int i = 0; i < numIntervals; i++) {
            Interval currentInterval = this->inputIntervals.at(i);
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

        return OPT[numIntervals-1];
    }

    int wlisLLP() {
        // Settings
        int par_enabled = 1;

        // std::vector<Interval> intervals = this->inputIntervals;
        int numIntervals = this->inputIntervals.size();

        // First, sort the intervals in ascending order of their finish times if not already sorted
        if (!sorted) {
            std::sort (this->inputIntervals.begin(), this->inputIntervals.end());
            #pragma omp parallel for if(par_enabled)
            for (int i = 0; i < numIntervals; i++) {
                this->inputIntervals.at(i).setId(i);
            }
        }

        // Next, calculate P(j), where P(j) is the rightmost interval i such that i and j are compatible (f_i <= s_j)
        // We can do this in parallel
        int P [numIntervals];
        #pragma omp parallel for if(par_enabled)
        for (int i = 0; i < numIntervals; i++) {
            Interval interval = this->inputIntervals.at(i);
            P[i] = getP(this->inputIntervals, interval);
        }

        // Then, we use an LLP algorithm to find the global state G
        int G [numIntervals+1];

        G[0] = 0;
        #pragma omp parallel for if(par_enabled)
        for (int i = 0; i < numIntervals; i++) {
            G[i+1] = 0;
        }

        bool diffExists = true;
        int GCalc [numIntervals];
        while (diffExists) {
            diffExists = false;
            #pragma omp parallel if(par_enabled)
            {
                #pragma omp for
                for (int i = 0; i < numIntervals; i++) {
                    GCalc[i] = std::max(this->inputIntervals.at(i).weight + G[P[i]+1], G[i]);
                    if (GCalc[i] != G[i+1]) {
                        diffExists = true;
                    }
                }

                #pragma omp for
                for (int i = 0; i < numIntervals; i++) {
                    G[i+1] = GCalc[i];
                }
            }
        }
        return G[numIntervals];
    }

public:

    void init(std::string inputFileName, bool shouldSort) {
        parseInput(inputFileName);

        this->sorted = shouldSort;
        if (shouldSort) {
            int numIntervals = inputIntervals.size();
            std::sort (inputIntervals.begin(), inputIntervals.end());
            for (int i = 0; i < numIntervals; i++) {
                inputIntervals.at(i).setId(i);
            }
        }
    }

    int run(std::string algoType) {
        if (algoType == "seq") {
            return wlisSeq();
        } else if (algoType == "par") {
            return wlisPar();
        } else if (algoType == "parllp") {
            return wlisLLP();
        } else {
            // Invalid algoType, print error
            return -1;
        }
    }

};

int main(int argc, char *argv[]) {

    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    std::cout.precision(8);

    int numTrials = 3;
    for (int i = 1; i < argc; i++) {
        WLIS wlis;

        std::cout << "-----------------------------------" << std::endl;
        std::cout << "WLIS for test: " << argv[i] << std::endl;
        for (int trial = 1; trial <= numTrials; trial++) {

            wlis.init(argv[i], true);
            auto t1 = high_resolution_clock::now();
            int seqResult = wlis.run("seq");
            auto t2 = high_resolution_clock::now();

            wlis.init(argv[i], true);
            auto t3 = high_resolution_clock::now();
            int parResult = wlis.run("par");
            auto t4 = high_resolution_clock::now();

            wlis.init(argv[i], true);
            auto t5 = high_resolution_clock::now();
            int parllpResult = wlis.run("parllp");
            auto t6 = high_resolution_clock::now();

            /* Getting number of milliseconds as an integer. */
            duration<double, std::milli> ms_double_seq = t2 - t1;
            duration<double, std::milli> ms_double_par = t4 - t3;
            duration<double, std::milli> ms_double_parllp = t6 - t5;

            if (seqResult == parResult && parResult == parllpResult) {
                std::cout << std::fixed << ms_double_seq.count() << "\t" << ms_double_par.count() << "\t" << ms_double_parllp.count() << std::endl;
            } else {
                std::cout << "Incorrect results: Seq: " << std::to_string(seqResult) << " Par: " << std::to_string(parResult) << " ParLLP: " << std::to_string(parllpResult) << std::endl;
            }
        }

        std::cout << "-----------------------------------" << std::endl;


    }
    return 0;
}
