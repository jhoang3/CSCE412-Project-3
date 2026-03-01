/**
 * @file Config.cpp
 * @brief Implementation of config loading and IP blocking.
 * @author Brady Nguyen
 * @date February 2026
 */

#include "Config.h"
#include <fstream>
#include <sstream>
#include <cstdint>
#include <algorithm>
#include <cctype>

namespace {

void trim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isspace(c); }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c) { return !std::isspace(c); }).base(), s.end());
}

bool parseIPRange(const std::string& s, uint32_t& outBase, int& outPrefix) {
    std::istringstream iss(s);
    int a, b, c, d;
    char ch;
    if (!(iss >> a >> ch && ch == '.' && iss >> b >> ch && ch == '.' && iss >> c >> ch && ch == '.' && iss >> d))
        return false;
    if (a < 0 || a > 255 || b < 0 || b > 255 || c < 0 || c > 255 || d < 0 || d > 255)
        return false;
    outBase = (static_cast<uint32_t>(a) << 24) | (static_cast<uint32_t>(b) << 16) |
              (static_cast<uint32_t>(c) << 8) | static_cast<uint32_t>(d);
    outPrefix = 32;
    if (iss >> ch && ch == '/') {
        if (!(iss >> outPrefix) || outPrefix < 0 || outPrefix > 32)
            return false;
    }
    return true;
}

uint32_t ipStringToU32(const std::string& ip) {
    uint32_t base;
    int prefix;
    if (!parseIPRange(ip, base, prefix))
        return 0;
    return base;
}

bool ipInRange(uint32_t ip, uint32_t base, int prefixLen) {
    if (prefixLen <= 0) return true;
    if (prefixLen >= 32) return ip == base;
    uint32_t mask = static_cast<uint32_t>(0xFFFFFFFF) << (32 - prefixLen);
    return (ip & mask) == (base & mask);
}

} // anonymous namespace

bool loadConfig(const std::string& path, Config& out) {
    std::ifstream f(path);
    if (!f) return false;

    std::string line;
    while (std::getline(f, line)) {
        trim(line);
        if (line.empty() || line[0] == '#') continue;

        std::string::size_type eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key = line.substr(0, eq);
        std::string value = line.substr(eq + 1);
        trim(key);
        trim(value);

        if (key == "scaleUpThreshold") {
            int v = 0;
            if (std::istringstream(value) >> v) out.scaleUpThreshold = v;
        } else if (key == "scaleDownThreshold") {
            int v = 0;
            if (std::istringstream(value) >> v) out.scaleDownThreshold = v;
        } else if (key == "cooldownCycles") {
            int v = 0;
            if (std::istringstream(value) >> v) out.cooldownCycles = v;
        } else if (key == "initialQueueMultiplier") {
            int v = 0;
            if (std::istringstream(value) >> v) out.initialQueueMultiplier = v;
        } else if (key == "blockedRange" || key == "blockedIP") {
            if (!value.empty()) out.blockedIPRanges.push_back(value);
        }
    }
    return true;
}

bool isIPBlocked(const Config& cfg, const std::string& ip) {
    uint32_t ipVal = ipStringToU32(ip);
    for (const std::string& rangeStr : cfg.blockedIPRanges) {
        uint32_t base;
        int prefix;
        if (!parseIPRange(rangeStr, base, prefix)) continue;
        if (ipInRange(ipVal, base, prefix)) return true;
    }
    return false;
}
