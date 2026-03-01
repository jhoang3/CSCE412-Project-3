/**
 * @file LoadBalancer.cpp
 * @brief Implementation of the load balancer (constructor, initialize, runCycle, scaleCluster, telemetry).
 * @author Brady Nguyen
 * @date February 2026
 */

#include "LoadBalancer.h"
#include "Config.h"
#include "Request.h"
#include <cstdlib>
#include <iostream>

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
    if (cooldown_timer > 0) {
        --cooldown_timer;
        return;
    }
    const size_t n = cluster.size();
    const size_t q = buffer.size();
    if (q > static_cast<size_t>(settings.scaleUpThreshold) * n) {
        cluster.emplace_back();
        ++scale_ups;
        cooldown_timer = settings.cooldownCycles;
    } else if (n > 1 && q < static_cast<size_t>(settings.scaleDownThreshold) * n) {
        cluster.pop_back();
        ++scale_downs;
        cooldown_timer = settings.cooldownCycles;
    }
}

void LoadBalancer::telemetry(std::ostream* log) {
    /* ANSI: \033[0m reset, \033[32m green, \033[31m red, \033[33m yellow */
    const char* reset = "\033[0m";
    const char* green = "\033[32m";
    const char* red = "\033[31m";
    std::ostream& out = std::cout;
    out << "clock=" << system_clock
        << " queue=" << buffer.size()
        << " servers=" << cluster.size()
        << " " << green << "processed=" << total_processed << reset
        << " " << red << "dropped=" << total_dropped << reset
        << "\n";
    if (log) {
        *log << "clock=" << system_clock
             << " queue=" << buffer.size()
             << " servers=" << cluster.size()
             << " processed=" << total_processed
             << " dropped=" << total_dropped
             << "\n";
    }
}
