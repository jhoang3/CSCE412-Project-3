/**
 * @file LoadBalancer.cpp
 * @brief Implementation of the load balancer (Parts 1–2: constructor, initialize, runCycle).
 * @author Brady Nguyen
 * @date February 2026
 */

#include "LoadBalancer.h"
#include "Config.h"
#include "Request.h"
#include <cstdlib>

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
    ++system_clock;

    /* Maybe add one new request this cycle (e.g. 50% chance). */
    if (std::rand() % 2 == 0) {
        Request r = generateRandomRequest();
        if (isIPBlocked(settings, r.IP_in)) {
            ++total_dropped;
        } else {
            buffer.push(r);
        }
    }

    /* Process each server: tick if busy (count completed jobs), else dispatch from queue if available. */
    for (WebServer& server : cluster) {
        if (server.isBusy()) {
            if (server.tick()) {
                ++total_processed;
            }
        } else if (!buffer.empty()) {
            Request r = buffer.front();
            buffer.pop();
            server.dispatch(r);
        }
    }

    scaleCluster();
    telemetry(log);
}

void LoadBalancer::scaleCluster() {
    /* Part 3: to be implemented */
}

void LoadBalancer::telemetry(std::ostream* log) {
    (void)log;
    /* Part 3: to be implemented */
}
