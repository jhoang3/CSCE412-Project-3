/**
 * @file LoadBalancer.cpp
 * @brief Implementation of the load balancer (Part 1: constructor and initialize).
 * @author Brady Nguyen
 * @date February 2026
 */

#include "LoadBalancer.h"
#include "Config.h"
#include "Request.h"

LoadBalancer::LoadBalancer(const Config& cfg) : settings(cfg) {}

void LoadBalancer::initialize(int numServers) {
    cluster.clear();
    cluster.resize(numServers);

    const size_t initialCount = numServers * static_cast<size_t>(settings.initialQueueMultiplier);
    for (size_t i = 0; i < initialCount; ++i) {
        Request r = generateRandomRequest();
        if (isIPBlocked(settings, r.IP_in)) {
            ++total_dropped;
        } else {
            buffer.push(r);
        }
    }
}

void LoadBalancer::runCycle(std::ostream* log) {
    (void)log;
    /* Part 2: to be implemented */
}

void LoadBalancer::scaleCluster() {
    /* Part 3: to be implemented */
}

void LoadBalancer::telemetry(std::ostream* log) {
    (void)log;
    /* Part 3: to be implemented */
}
