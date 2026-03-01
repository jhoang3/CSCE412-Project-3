/**
 * @file main.cpp
 * @brief Driver for the load balancer simulation.
 * @author Brady Nguyen
 * @date February 2026
 */

#include "Config.h"
#include "LoadBalancer.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    int numServers = 0;
    int runTime = 0;
    std::string configPath = "config.txt";

    std::cout << "Enter number of web servers: ";
    std::cin >> numServers;
    std::cout << "Enter run time (clock cycles): ";
    std::cin >> runTime;

    if (numServers <= 0 || runTime <= 0) {
        std::cerr << "Invalid input: servers and run time must be positive.\n";
        return 1;
    }

    Config cfg;
    if (!loadConfig(configPath, cfg)) {
        std::cout << "Config file '" << configPath << "' not found; using defaults.\n";
    }

    LoadBalancer lb(cfg);
    lb.initialize(numServers);

    std::ofstream logFile("loadbalancer.log");
    std::ostream* log = logFile.is_open() ? &logFile : nullptr;

    lb.logStart(log);

    for (int cycle = 0; cycle < runTime; ++cycle) {
        lb.runCycle(log);
    }

    std::cout << "\n--- Summary ---\n";
    std::cout << "Total processed: " << lb.getTotalProcessed() << "\n";
    std::cout << "Total dropped:   " << lb.getTotalDropped() << "\n";
    std::cout << "Scale-ups:       " << lb.getScaleUps() << "\n";
    std::cout << "Scale-downs:     " << lb.getScaleDowns() << "\n";
    std::cout << "Final servers:   " << lb.getServerCount() << "\n";
    std::cout << "Final queue:     " << lb.getQueueSize() << "\n";
    std::cout << "\n--- End Status ---\n";
    std::cout << "Remaining requests in queue: " << lb.getQueueSize() << "\n";
    std::cout << "Active servers:   " << lb.getActiveServerCount() << "\n";
    std::cout << "Inactive servers: " << lb.getInactiveServerCount() << "\n";
    std::cout << "Rejected/discarded requests: " << lb.getTotalDropped() << "\n";

    if (log) {
        logFile << "\n--- Summary ---\n";
        logFile << "Total processed: " << lb.getTotalProcessed() << "\n";
        logFile << "Total dropped:   " << lb.getTotalDropped() << "\n";
        logFile << "Scale-ups:       " << lb.getScaleUps() << "\n";
        logFile << "Scale-downs:     " << lb.getScaleDowns() << "\n";
        logFile << "Final servers:   " << lb.getServerCount() << "\n";
        logFile << "Ending queue size: " << lb.getQueueSize() << "\n";
        logFile << "\n--- End Status ---\n";
        logFile << "Remaining requests in queue: " << lb.getQueueSize() << "\n";
        logFile << "Active servers:   " << lb.getActiveServerCount() << "\n";
        logFile << "Inactive servers: " << lb.getInactiveServerCount() << "\n";
        logFile << "Rejected/discarded requests: " << lb.getTotalDropped() << "\n";
    }

    return 0;
}
