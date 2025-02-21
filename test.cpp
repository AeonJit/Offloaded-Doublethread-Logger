
// work.cpp
#include "logger.h"
#include <vector>
#include <chrono>
void doHeavyWork(Logger& logger) {
    std::vector<int> data(10000,0);
    for (unsigned i = 1; i < data.size(); ++i) {
        data[i] = i * 2 ^ data[i-1];  // Some computation
        logger.debug("Computed data: " + std::to_string(data[i]));
        if (i % 1000 == 0) {
            logger.info("Processed " + std::to_string(i) + " items");
        }
        if(data[i] & 1) {
            logger.warn("Odd number detected: " + std::to_string(data[i]));
        }

        if (data[i] * data[i-1] & 3) {
            logger.error("Data corruption detected: " + std::to_string(data[i]) + " " + std::to_string(data[i-1]));
        }

    }
    logger.info("Heavy work completed");
}

void interferingLogggers(Logger& logger) {
    logger.warn("Interfering logger started");
    Logger nestedLogger;
    nestedLogger.info("Nested logger started");
    doHeavyWork(nestedLogger);
    nestedLogger.info("Nested logger ending");
    
}

int main() {
    Logger logger;
    logger.info("Starting heavy work");
    interferingLogggers(logger);
    // doHeavyWork(logger);
    
    logger.info("Program ending");
    return 0;  // Logger destructor runs here, joining the logging thread
}
