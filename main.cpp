/**
 * @file main.cpp
 * @brief Driver for the load balancer simulation (single LB or Switch with job-type routing).
 * @author Brady Nguyen
 * @date February 2026
 */

#include "Config.h"
#include "LoadBalancer.h"
#include "Switch.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    int runTime = 0;
    std::string configPath = "config.txt";

    std::cout << "Use Switch (job-type routing: S->Streaming LB, P->Processing LB)? 1=yes, 0=no: ";
    int useSwitch = 0;
    std::cin >> useSwitch;

    if (useSwitch != 0 && useSwitch != 1) {
        std::cerr << "Invalid input: enter 1 for Switch mode or 0 for single load balancer.\n";
        return 1;
    }

    std::cout << "Enter run time (clock cycles): ";
    std::cin >> runTime;
    if (runTime <= 0) {
        std::cerr << "Invalid input: run time must be positive.\n";
        return 1;
    }

    Config cfg;
    if (!loadConfig(configPath, cfg)) {
        std::cout << "Config file '" << configPath << "' not found; using defaults.\n";
    }

    std::ofstream logFile("loadbalancer.log");
    std::ostream* log = logFile.is_open() ? &logFile : nullptr;

    if (useSwitch == 1) {
        int numStreaming = 0, numProcessing = 0;
        std::cout << "Enter number of web servers for Streaming (S): ";
        std::cin >> numStreaming;
        std::cout << "Enter number of web servers for Processing (P): ";
        std::cin >> numProcessing;
        if (numStreaming < 0 || numProcessing < 0) {
            std::cerr << "Invalid input: server counts must be non-negative.\n";
            return 1;
        }

        Switch sw(cfg);
        sw.initialize(numStreaming, numProcessing);

        if (log) {
            logFile << "--- Switch mode: Streaming LB + Processing LB ---\n";
            logFile << "Streaming servers: " << numStreaming << ", Processing servers: " << numProcessing << "\n";
        }

        for (int cycle = 0; cycle < runTime; ++cycle) {
            sw.runCycle(log);
        }

        const LoadBalancer& slb = sw.getStreamingLB();
        const LoadBalancer& plb = sw.getProcessingLB();

        std::cout << "\n--- Summary (Switch mode) ---\n";
        std::cout << "Streaming LB:  processed=" << slb.getTotalProcessed() << " dropped=" << slb.getTotalDropped()
                  << " scale-ups=" << slb.getScaleUps() << " scale-downs=" << slb.getScaleDowns()
                  << " final_servers=" << slb.getServerCount() << " final_queue=" << slb.getQueueSize() << "\n";
        std::cout << "Processing LB: processed=" << plb.getTotalProcessed() << " dropped=" << plb.getTotalDropped()
                  << " scale-ups=" << plb.getScaleUps() << " scale-downs=" << plb.getScaleDowns()
                  << " final_servers=" << plb.getServerCount() << " final_queue=" << plb.getQueueSize() << "\n";
        std::cout << "Combined:      processed=" << (slb.getTotalProcessed() + plb.getTotalProcessed())
                  << " dropped=" << (slb.getTotalDropped() + plb.getTotalDropped()) << "\n";

        if (log) {
            logFile << "\n--- Summary (Switch mode) ---\n";
            logFile << "Streaming LB:  processed=" << slb.getTotalProcessed() << " dropped=" << slb.getTotalDropped()
                    << " scale-ups=" << slb.getScaleUps() << " scale-downs=" << slb.getScaleDowns()
                    << " final_servers=" << slb.getServerCount() << " final_queue=" << slb.getQueueSize() << "\n";
            logFile << "Processing LB: processed=" << plb.getTotalProcessed() << " dropped=" << plb.getTotalDropped()
                    << " scale-ups=" << plb.getScaleUps() << " scale-downs=" << plb.getScaleDowns()
                    << " final_servers=" << plb.getServerCount() << " final_queue=" << plb.getQueueSize() << "\n";
            logFile << "Combined:      processed=" << (slb.getTotalProcessed() + plb.getTotalProcessed())
                    << " dropped=" << (slb.getTotalDropped() + plb.getTotalDropped()) << "\n";
        }
    } else {
        int numServers = 0;
        std::cout << "Enter number of web servers: ";
        std::cin >> numServers;
        if (numServers <= 0) {
            std::cerr << "Invalid input: servers must be positive.\n";
            return 1;
        }

        LoadBalancer lb(cfg);
        lb.initialize(numServers);
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
    }

    return 0;
}
