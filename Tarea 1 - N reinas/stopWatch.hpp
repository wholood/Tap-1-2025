#ifndef STOP_WATCH
#define STOP_WATCH

#include<chrono>

using namespace std;
using namespace std::chrono;

class StopWatch {

	class ElapsedTime {
		steady_clock::time_point begin;
		steady_clock::time_point end;

	public:
		ElapsedTime(steady_clock::time_point begin, steady_clock::time_point end) :begin(begin), end(end) {}

		constexpr long long getElapsedTimeMicroSeconds() {
			return duration_cast<microseconds>(end - begin).count();
		}

		constexpr long long getElapsedTimeMiliSeconds() {
			return duration_cast<milliseconds>(end - begin).count();
		}

		constexpr long long getElapsedTimeNanoSeconds() {
			return duration_cast<nanoseconds>(end - begin).count();
		}
	};


	class RunningWatch {
		steady_clock::time_point begin;

	public:
		RunningWatch() :begin(steady_clock::now()) {}

		ElapsedTime stop() {
			steady_clock::time_point end = steady_clock::now();

			return ElapsedTime(begin, end);
		}
	};

public:
	static RunningWatch start() { return RunningWatch(); }
};

#endif // !STOP_WATCH