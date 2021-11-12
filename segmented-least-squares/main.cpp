#include <iostream>
#include <chrono>
#include <vector>
#include <utility>
#include "sls.cpp"

void segmentedLeastSquaresSeq() {}
void segmentedLeastSquaresPar() {}

int main() {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::chrono::milliseconds;

	std::vector<std::pair<int, int>> points;
	int N;
	int C;
	std::cin >> N >> C;
	for(int i = 0; i<N; ++i) {
		int a, b;
		std::cin >> a >> b;
		points.push_back({a, b});
	}
	
	Solver s;
	s.init(N, points, C);
	for(int i = 0; i<5; ++i) {
		s.reset();

		auto t1 = high_resolution_clock::now();
		double seq = s.sequential();
		auto t2 = high_resolution_clock::now();

		s.reset();
		auto t3 = high_resolution_clock::now();
		double par = s.parallel();
		auto t4 = high_resolution_clock::now();

		/* Getting number of milliseconds as an integer. */
		duration<double, std::milli> ms_double_seq = t2 - t1;

		duration<double, std::milli> ms_double_par = t4 - t3;

		
		std::cout << N << ",";
		std::cout << ms_double_seq.count() << "ms,";
		std::cout << ms_double_par.count() << "ms\n";

	}
    return 0;
}
