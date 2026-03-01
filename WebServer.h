/**
 * @file WebServer.h
 * @brief WebServer class that processes requests for the load balancer simulation.
 * @author Brady Nguyen
 * @date February 2026
 */

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "Request.h"

/**
 * @class WebServer
 * @brief Represents a single server that accepts requests and processes them over multiple clock cycles.
 */
class WebServer {
public:
    /**
     * @brief Assigns a request to this server and marks it busy.
     * @param r The request to process.
     */
    void dispatch(const Request& r);

    /**
     * @brief Advances one clock cycle; decrements the job timer and marks idle when done.
     * @return true if a job was completed this cycle, false otherwise.
     */
    bool tick();

    /**
     * @brief Returns whether the server is currently processing a request.
     * @return true if busy, false if idle.
     */
    bool isBusy() const;

private:
    Request active_job{};     /**< The request currently being processed */
    int countdown_timer = 0;  /**< Cycles remaining until current job completes */
    bool busy = false;        /**< Whether the server is processing a request */
};

#endif /* WEBSERVER_H */
