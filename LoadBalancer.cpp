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
        Request r = generateRandomRequest(settings);
        if (isIPBlocked(settings, r.IP_in)) {
            ++total_dropped;
        } else {
            buffer.push(r);
        }
    }
}

void LoadBalancer::initializeServersOnly(int numServers) {
    cluster.clear();
    cluster.resize(numServers);
}

void LoadBalancer::enqueueRequest(const Request& r) {
    if (isIPBlocked(settings, r.IP_in)) {
        ++total_dropped;
    } else {
        buffer.push(r);
    }
}

void LoadBalancer::logStart(std::ostream* log) const {
    if (!log) return;
    *log << "Starting queue size: " << buffer.size() << "\n";
    *log << "Task time range: " << settings.taskTimeMin << "-" << settings.taskTimeMax << " cycles\n";
}

void LoadBalancer::runCycle(std::ostream* log, bool addRandomRequest) {
    ++system_clock;

    int added_this_cycle = 0;
    if (addRandomRequest && (std::rand() % 100) < settings.requestArrivalChance) {
        Request r = generateRandomRequest(settings);
        if (isIPBlocked(settings, r.IP_in)) {
            ++total_dropped;
        } else {
            buffer.push(r);
            added_this_cycle = 1;
        }
    }

    int dispatched_this_cycle = 0;
    int completed_this_cycle = 0;
    /* Process each server: tick if busy (count completed jobs), else dispatch from queue if available. */
    for (WebServer& server : cluster) {
        if (server.isBusy()) {
            if (server.tick()) {
                ++total_processed;
                ++completed_this_cycle;
            }
        } else if (!buffer.empty()) {
            Request r = buffer.front();
            buffer.pop();
            server.dispatch(r);
            ++dispatched_this_cycle;
        }
    }

    const size_t servers_before = cluster.size();
    scaleCluster();
    const size_t servers_after = cluster.size();

    telemetry(log, added_this_cycle, dispatched_this_cycle, completed_this_cycle);

    if (log && servers_after != servers_before) {
        if (servers_after > servers_before)
            *log << "  [scale up: " << servers_before << " -> " << servers_after << " servers]\n";
        else
            *log << "  [scale down: " << servers_before << " -> " << servers_after << " servers]\n";
    }
}

/* Spec §10: Goal keep queue between 50× and 80× servers. If > 80× add 1 server, wait n cycles. If < 50× remove 1 server, wait n. */
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
    } else if (n > 1 && !cluster.back().isBusy() && q < static_cast<size_t>(settings.scaleDownThreshold) * n) {
        cluster.pop_back();
        ++scale_downs;
        cooldown_timer = settings.cooldownCycles;
    }
}

size_t LoadBalancer::getActiveServerCount() const {
    size_t n = 0;
    for (const WebServer& s : cluster)
        if (s.isBusy()) ++n;
    return n;
}

void LoadBalancer::telemetry(std::ostream* log, int added, int dispatched, int completed) {
    const size_t active = getActiveServerCount();
    const size_t inactive = cluster.size() - active;
    /* ANSI: \033[0m reset, \033[32m green, \033[31m red, \033[33m yellow */
    const char* reset = "\033[0m";
    const char* green = "\033[32m";
    const char* red = "\033[31m";
    std::ostream& out = std::cout;
    out << "clock=" << system_clock
        << " queue=" << buffer.size()
        << " servers=" << cluster.size()
        << " active=" << active
        << " inactive=" << inactive
        << " added=" << added
        << " dispatched=" << dispatched
        << " completed=" << completed
        << " " << green << "processed=" << total_processed << reset
        << " " << red << "dropped=" << total_dropped << reset
        << "\n";
    if (log) {
        *log << "clock=" << system_clock
             << " queue=" << buffer.size()
             << " servers=" << cluster.size()
             << " active=" << active
             << " inactive=" << inactive
             << " added=" << added
             << " dispatched=" << dispatched
             << " completed=" << completed
             << " processed=" << total_processed
             << " dropped=" << total_dropped
             << "\n";
    }
}
