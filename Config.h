/**
 * @file Config.h
 * @brief Configuration struct and load/block helpers for the load balancer.
 * @author Brady Nguyen
 * @date February 2026
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>

/**
 * @struct Config
 * @brief Holds load balancer settings loaded from a config file.
 */
struct Config {
    int scaleUpThreshold = 80;       /**< Add server when queue size > this * server count */
    int scaleDownThreshold = 50;    /**< Remove server when queue size < this * server count */
    int cooldownCycles = 10;        /**< Cycles to wait after scaling before scaling again */
    int initialQueueMultiplier = 100; /**< Initial queue size = servers * this */
    std::vector<std::string> blockedIPRanges; /**< IP ranges to block (e.g. "192.168.0.0/24") */
};

/**
 * @brief Loads configuration from a file into the given struct.
 * @param path Path to the config file (e.g. "config.txt").
 * @param out Config struct to fill; existing values used as defaults if key missing.
 * @return true if file was opened and parsed, false on open error (out unchanged).
 */
bool loadConfig(const std::string& path, Config& out);

/**
 * @brief Returns whether an IP address is in any blocked range in the config.
 * @param cfg Loaded configuration (blockedIPRanges used).
 * @param ip Dotted-decimal IPv4 address (e.g. "192.168.1.1").
 * @return true if ip is blocked, false otherwise.
 */
bool isIPBlocked(const Config& cfg, const std::string& ip);

#endif /* CONFIG_H */
