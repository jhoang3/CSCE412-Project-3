 /**
 * @file Switch.cpp
 * @brief Implementation of the Switch (job-type routing to two load balancers).
 * @author Brady Nguyen
 * @date February 2026
 */

#include "Switch.h"
#include "Request.h"
#include <cstdlib>

Switch::Switch(const Config& cfg)
    : settings_(cfg),
      streamingLB_(cfg),
      processingLB_(cfg) {}

void Switch::initialize(int numStreamingServers, int numProcessingServers) {
    streamingLB_.initializeServersOnly(numStreamingServers);
    processingLB_.initializeServersOnly(numProcessingServers);

    const int mult = settings_.initialQueueMultiplier;
    const size_t totalInitial = (numStreamingServers + numProcessingServers) * static_cast<size_t>(mult);
    for (size_t i = 0; i < totalInitial; ++i) {
        Request r = generateRandomRequest(settings_);
        if (r.job_type == 'S')
            streamingLB_.enqueueRequest(r);
        else
            processingLB_.enqueueRequest(r);
    }
}

void Switch::runCycle(std::ostream* log) {
    if ((std::rand() % 100) < settings_.requestArrivalChance) {
        Request r = generateRandomRequest(settings_);
        if (r.job_type == 'S')
            streamingLB_.enqueueRequest(r);
        else
            processingLB_.enqueueRequest(r);
    }
    streamingLB_.runCycle(log, false);
    processingLB_.runCycle(log, false);
}
