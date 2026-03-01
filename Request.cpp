/**
 * @file Request.cpp
 * @brief Implementation of random request generation.
 * @author Brady Nguyen
 * @date February 2026
 */

#include "Request.h"
#include "Config.h"
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

Request generateRandomRequest(const Config& cfg) {
    Request r;
    r.IP_in = randomIP();
    r.IP_out = randomIP();
    int lo = cfg.taskTimeMin;
    int hi = cfg.taskTimeMax;
    if (lo > hi) { int t = lo; lo = hi; hi = t; }
    r.cycles_needed = randomInRange(lo, hi);
    r.job_type = (std::rand() % 2 == 0) ? 'P' : 'S';
    return r;
}
