#ifndef KIRAN_RAYTRACER_PROFILER
#define KIRAN_RAYTRACER_PROFILER

#include <string>

#include <map>
#include <vector>

class Profiler {
public:
	enum {
		COUNTER_PHOTON = 0,
		COUNTER_RAY    = 1,
	};

	Profiler(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
	~Profiler();

	void StartTask(const std::string&, unsigned int);
	void StopTask(const std::string&, unsigned int);

	void IncCounter(unsigned int ctrType, unsigned int thread, unsigned int depth, unsigned int type) {
		switch (ctrType) {
			case COUNTER_PHOTON: { pCounters[thread][depth][type] += 1; } break;
			case COUNTER_RAY:    { rCounters[thread][depth][type] += 1; } break;
			default: {} break;
		}
	}


private:
	std::vector<std::vector<std::vector<unsigned int> > > rCounters;
	std::vector<std::vector<std::vector<unsigned int> > > pCounters;

	std::map<std::string, unsigned int> taskTimes;
};

#endif
