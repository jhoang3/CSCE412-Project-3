/**
 * @file WebServer.cpp
 * @brief Implementation of WebServer methods.
 * @author Brady Nguyen
 * @date February 2026
 */

#include "WebServer.h"

void WebServer::dispatch(const Request& r) {
    active_job = r;
    countdown_timer = r.cycles_needed;
    busy = true;
}

bool WebServer::tick() {
    if (!busy) return false;
    --countdown_timer;
    if (countdown_timer <= 0) {
        busy = false;
        return true;
    }
    return false;
}

bool WebServer::isBusy() const {
    return busy;
}
