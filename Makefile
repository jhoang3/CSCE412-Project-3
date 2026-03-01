# Load balancer simulation - Makefile
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
SRCS = main.cpp Request.cpp Config.cpp WebServer.cpp LoadBalancer.cpp
TARGET = loadbalancer

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET) loadbalancer.log

.PHONY: clean
