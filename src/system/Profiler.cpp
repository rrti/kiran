#include <sstream>
#include <iostream>

#include "./Profiler.hpp"

Profiler::Profiler(
	unsigned int numThreads,
	unsigned int maxDepthR,
	unsigned int maxDepthP,
	unsigned int numTypesR,
	unsigned int numTypesP
) {
	rCounters.resize(numThreads);
	pCounters.resize(numThreads);

	for (unsigned int i = 0; i < numThreads; i++) {
		rCounters[i].resize(maxDepthR, std::vector<unsigned int>(numTypesR, 0));
		pCounters[i].resize(maxDepthP, std::vector<unsigned int>(numTypesP, 0));
	}
}

Profiler::~Profiler() {
	std::stringstream ss;
	ss << "[Profiler::~Profiler]" << std::endl;
	ss << "\tmaximum (ray   ) recursion depth: " << (rCounters[0].size()) << std::endl;
	ss << "\tmaximum (photon) recursion depth: " << (pCounters[0].size()) << std::endl;
	ss << std::endl;

	unsigned int numRays = 0;
	unsigned int numPhotons = 0;

	for (size_t thread = 0; thread < rCounters.size(); thread++) {
		ss << "\tthread: " << thread << std::endl;

		for (size_t depth = 0; depth < rCounters[thread].size(); depth++) {
			ss << "\t\tray depth: " << depth << std::endl;

			for (size_t type = 0; type < rCounters[thread][depth].size(); type++) {
				numRays += rCounters[thread][depth][type];

				ss << "\t\t\ttype: " << type;
				ss << ", number of rays: " << rCounters[thread][depth][type];
				ss << std::endl;
			}

			rCounters[thread][depth].clear();
		}

		ss << std::endl;
		rCounters[thread].clear();
	}

	for (size_t thread = 0; thread < pCounters.size(); thread++) {
		ss << "\tthread: " << thread << std::endl;

		for (size_t depth = 0; depth < pCounters[thread].size(); depth++) {
			ss << "\t\tphoton depth: " << depth << std::endl;

			for (size_t type = 0; type < pCounters[thread][depth].size(); type++) {
				numPhotons += pCounters[thread][depth][type];

				ss << "\t\t\ttype: " << type;
				ss << ", number of photons: " << pCounters[thread][depth][type];
				ss << std::endl;
			}

			pCounters[thread][depth].clear();
		}

		ss << std::endl;
		pCounters[thread].clear();
	}

	ss << "\ttotal ray-count: " << numRays << std::endl;
	ss << "\ttotal photon-count: " << numPhotons << std::endl;
	ss << std::endl;

	for (std::map<std::string, unsigned int>::const_iterator it = taskTimes.begin(); it != taskTimes.end(); it++) {
		ss << "\ttask: \"" << it->first << "\", execution time: " << (it->second / 1000.0f) << "s" << std::endl;
	}

	std::cout << ss.str();

	rCounters.clear();
	pCounters.clear();
	taskTimes.clear();
}


void Profiler::StartTask(const std::string& taskName, unsigned int startTime) {
	taskTimes[taskName] = startTime;
}

void Profiler::StopTask(const std::string& taskName, unsigned int stopTime) {
	taskTimes[taskName] = stopTime - taskTimes[taskName];
}
