/**
 * @file Request.h
 * @brief Request struct and random request generator for load balancer simulation.
 * @author Brady Nguyen
 * @date February 2026
 */

#ifndef REQUEST_H
#define REQUEST_H

#include "Config.h"
#include <string>

/**
 * @struct Request
 * @brief Represents a single web request with IP addresses, processing time, and job type.
 */
struct Request {
    std::string IP_in;        /**< Requester IP address */
    std::string IP_out;       /**< Response destination IP address */
    int cycles_needed;        /**< Clock cycles required to process this request */
    char job_type;            /**< 'P' = Processing, 'S' = Streaming */
};

/**
 * @brief Generates a random request with random IPs, cycle count, and job type.
 * @param cfg Config providing taskTimeMin and taskTimeMax for cycles_needed range.
 * @return A new Request with random IP_in, IP_out, cycles_needed in [taskTimeMin, taskTimeMax], and job_type ('P' or 'S').
 */
Request generateRandomRequest(const Config& cfg);

#endif /* REQUEST_H */
