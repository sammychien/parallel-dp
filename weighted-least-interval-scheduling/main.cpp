#include <bits/stdc++.h>
#include <chrono>

void weightedLISSeq() {}
void weightedLISPar() {}

int main() {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto t1 = high_resolution_clock::now();
    weightedLISSeq();
    auto t2 = high_resolution_clock::now();

    auto t3 = high_resolution_clock::now();
    weightedLISPar();
    auto t4 = high_resolution_clock::now();

    /* Getting number of milliseconds as an integer. */
    duration<double, std::milli> ms_double_seq = t2 - t1;
    
    duration<double, std::milli> ms_double_par = t4 - t3;
    

    std::cout << ms_double_seq.count() << "ms\n";
    std::cout << ms_double_par.count() << "ms\n";

    return 0;
}
