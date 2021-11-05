#include <iostream>
#include <chrono>

void wlisSeq() {}
void wlisParBF() {}
void wlisParDP() {}

int main() {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto t1 = high_resolution_clock::now();
    wlisSeq();
    auto t2 = high_resolution_clock::now();

    auto t3 = high_resolution_clock::now();
    wlisParBF();
    auto t4 = high_resolution_clock::now();

    auto t5 = high_resolution_clock::now();
    wlisParDP();
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
