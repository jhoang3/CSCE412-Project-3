/**
 * @file Request.h
 * @brief Request struct and random request generator for load balancer simulation.
 * @author Brady Nguyen
 * @date February 2026
 */

#ifndef REQUEST_H
#define REQUEST_H

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
 * @return A new Request with random IP_in, IP_out, cycles_needed (e.g. 1–100), and job_type ('P' or 'S').
 */
Request generateRandomRequest();

#endif /* REQUEST_H */
