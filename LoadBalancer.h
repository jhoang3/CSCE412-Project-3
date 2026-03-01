/**
 * @file LoadBalancer.h
 * @brief Load balancer that manages a request queue and cluster of web servers.
 * @author Brady Nguyen
 * @date February 2026
 */

#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include "Config.h"
#include "Request.h"
#include "WebServer.h"
#include <queue>
#include <vector>
#include <iosfwd>

/**
 * @class LoadBalancer
 * @brief Orchestrates request queue, web servers, scaling, firewall, and telemetry.
 */
class LoadBalancer {
public:
    /**
     * @brief Constructs the load balancer with the given configuration.
     * @param cfg Configuration (thresholds, cooldown, blocked IPs).
     */
    explicit LoadBalancer(const Config& cfg);

    /**
     * @brief Creates web servers and fills the queue with initial requests.
     * @param numServers Number of web servers to create.
     */
    void initialize(int numServers);

    /**
     * @brief Creates web servers only; does not fill the queue (used by Switch for job-type routing).
     * @param numServers Number of web servers to create.
     */
    void initializeServersOnly(int numServers);

    /**
     * @brief Enqueues one request after firewall check (used by Switch to route by job type).
     * @param r Request to enqueue; if IP is blocked, total_dropped is incremented instead.
     */
    void enqueueRequest(const Request& r);

    /**
     * @brief Writes starting queue size and task time range to the log (call once after initialize).
     * @param log If non-null, starting info is written here.
     */
    void logStart(std::ostream* log) const;

    /**
     * @brief Runs one simulation cycle: clock, optionally new request, process servers, scale, telemetry.
     * @param log If non-null, telemetry line is also written here (no ANSI codes).
     * @param addRandomRequest If true, may add one random request this cycle; if false, only process (for Switch mode).
     */
    void runCycle(std::ostream* log = nullptr, bool addRandomRequest = true);

    /** @brief Current system clock (cycles elapsed). */
    long getClock() const { return system_clock; }
    /** @brief Current queue size. */
    size_t getQueueSize() const { return buffer.size(); }
    /** @brief Current number of web servers. */
    size_t getServerCount() const { return cluster.size(); }
    /** @brief Total requests processed so far. */
    int getTotalProcessed() const { return total_processed; }
    /** @brief Total requests dropped (firewall) so far. */
    int getTotalDropped() const { return total_dropped; }
    /** @brief Number of times a server was added. */
    int getScaleUps() const { return scale_ups; }
    /** @brief Number of times a server was removed. */
    int getScaleDowns() const { return scale_downs; }
    /** @brief Number of servers currently busy (processing a request). */
    size_t getActiveServerCount() const;
    /** @brief Number of servers currently idle. */
    size_t getInactiveServerCount() const { return getServerCount() - getActiveServerCount(); }

private:
    /** @brief Adds or removes a server based on queue size and cooldown. */
    void scaleCluster();
    /** @brief Prints and optionally logs one line of telemetry (clock, queue, servers, active, inactive, added, dispatched, completed, processed, dropped). */
    void telemetry(std::ostream* log, int added, int dispatched, int completed);

    Config settings;                    /**< Loaded configuration */
    std::queue<Request> buffer;          /**< Pending requests */
    std::vector<WebServer> cluster;      /**< Web server pool */
    long system_clock = 0;               /**< Current cycle count */
    int cooldown_timer = 0;              /**< Cycles until next scale decision */
    int total_processed = 0;             /**< Requests completed so far */
    int total_dropped = 0;               /**< Requests blocked by firewall */
    int scale_ups = 0;                   /**< Number of servers added */
    int scale_downs = 0;                 /**< Number of servers removed */
};

#endif /* LOADBALANCER_H */
