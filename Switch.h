/**
 * @file Switch.h
 * @brief Switch that routes requests by job type (Streaming vs Processing) to two load balancers.
 * @author Brady Nguyen
 * @date February 2026
 */

#ifndef SWITCH_H
#define SWITCH_H

#include "Config.h"
#include "LoadBalancer.h"
#include "Request.h"
#include <iosfwd>

/**
 * @class Switch
 * @brief Routes incoming requests by job_type to a Streaming LB ('S') or Processing LB ('P'); runs both LBs each cycle and exposes per-LB stats.
 */
class Switch {
public:
    /**
     * @brief Constructs the switch with shared configuration for both load balancers.
     * @param cfg Configuration (thresholds, cooldown, blocked IPs, task time range).
     */
    explicit Switch(const Config& cfg);

    /**
     * @brief Initializes both load balancers with the given server counts and fills queues with pre-routed requests.
     * @param numStreamingServers Number of web servers for the Streaming (S) load balancer.
     * @param numProcessingServers Number of web servers for the Processing (P) load balancer.
     */
    void initialize(int numStreamingServers, int numProcessingServers);

    /**
     * @brief Runs one cycle: generate one request (50% chance), route by job_type to the appropriate LB, then run both LBs (no internal request generation).
     * @param log If non-null, each LB may write telemetry here.
     */
    void runCycle(std::ostream* log = nullptr);

    /** @brief Streaming load balancer (job_type 'S'). */
    const LoadBalancer& getStreamingLB() const { return streamingLB_; }
    /** @brief Processing load balancer (job_type 'P'). */
    const LoadBalancer& getProcessingLB() const { return processingLB_; }

private:
    Config settings_;
    LoadBalancer streamingLB_;
    LoadBalancer processingLB_;
};

#endif /* SWITCH_H */
