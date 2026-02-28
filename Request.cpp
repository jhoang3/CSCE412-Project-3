/**
 * @file Request.cpp
 * @brief Implementation of random request generation.
 * @author Brady Nguyen
 * @date February 2026
 */

#include "Request.h"
#include <cstdlib>
#include <ctime>
#include <sstream>

namespace {

/** @brief Returns a random integer in [min, max] (inclusive). */
int randomInRange(int min, int max) {
    return min + (std::rand() % (max - min + 1));
}

/** @brief Generates a random dotted-decimal IPv4 address string. */
std::string randomIP() {
    std::ostringstream oss;
    oss << randomInRange(1, 223) << '.' << randomInRange(0, 255) << '.'
        << randomInRange(0, 255) << '.' << randomInRange(1, 254);
    return oss.str();
}

} // anonymous namespace

Request generateRandomRequest() {
    Request r;
    r.IP_in = randomIP();
    r.IP_out = randomIP();
    r.cycles_needed = randomInRange(1, 100);
    r.job_type = (std::rand() % 2 == 0) ? 'P' : 'S';
    return r;
}
