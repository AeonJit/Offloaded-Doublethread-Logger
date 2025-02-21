// logger.h
#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>
#define _LOGGER_UNUSED(var) ((void)var)
class Logger {
public:
    Logger() : logFilename(generateLogFilename()),  stopLogging(false) {
        logFile.open(logFilename, std::ios::app);
        if (!logFile.is_open()) {
            std::cerr << "Failed to open log file: " << logFilename << std::endl;
        }
        
        logThread = std::thread(&Logger::processLogs, this);
    }

    ~Logger() {
        stopLogging = true;
        logCondition.notify_all();
        if (logThread.joinable()) {
            logThread.join();
        }
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void info(const std::string& message)   { log("INFO ", message, GREEN); }
    void warn(const std::string& message)   { log("WARN ", message, YELLOW); }
    void error(const std::string& message)  { log("ERROR", message, RED); }
    #ifdef DEBUG
    void debug(const std::string& message)  { log("DEBUG", message, BLUE); }
    #else
    void debug(const std::string& message)  { _LOGGER_UNUSED(message); }
    #endif

private:
    std::ofstream logFile;
    const std::string logFilename;
    std::queue<std::string> logQueue;
    std::mutex queueMutex;
    std::condition_variable logCondition;
    std::thread logThread;
    std::atomic<bool> stopLogging;

    const std::string RESET   = "\033[0m";
    const std::string RED     = "\033[91m";  // Bright red
    const std::string YELLOW  = "\033[93m";  // Bright yellow
    const std::string GREEN   = "\033[92m";  // Bright green
    const std::string BLUE    = "\033[94m";  // Bright blue

    static std::string generateLogFilename() {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);

        std::ostringstream filename;
        filename << std::put_time(&tm, "%Y-%m-%d__%H:%M:%S") << ".log";
        return filename.str();
    }

    void log(const std::string& level, const std::string& message, const std::string& color) {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);

        std::ostringstream timeStream;
        timeStream << std::put_time(&tm, "[%Y-%m-%d %H:%M:%S]");
        std::string logMessage = timeStream.str() + " [" + level + "] " + message + "\n";

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            logQueue.push(logMessage);
        }

        logCondition.notify_one();
        std::cout << color << logMessage << RESET;
    }

    void processLogs() {
        while (!stopLogging || !logQueue.empty()) {
            std::unique_lock<std::mutex> lock(queueMutex);
            logCondition.wait(lock, [this] { return !logQueue.empty() || stopLogging; });

            while (!logQueue.empty()) {
                if (logFile.is_open()) {
                    logFile << logQueue.front();
                }
                logQueue.pop();
            }
            if (logFile.is_open()) {
                logFile.flush();
            }
        }
    }
};

#endif // LOGGER_H
