#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include <mpi.h>

class Logger {
public:
    enum class Level {
        ERROR,
        WARNING,
        INFO,
        DEBUG
    };

 
    Logger(int rank = 0, const std::string& filename = "")
        : rank_(rank), log_to_file_(!filename.empty()) 
    {
        if (log_to_file_) {
            file_.open(filename, std::ios::out | std::ios::app);
            if (!file_) {
                std::cerr << "Logger: Failed to open log file: " << filename << std::endl;
                log_to_file_ = false;
            }
        }
    }

    ~Logger() {
        if (file_.is_open()) {
            file_.close();
        }
    }

    
    void set_level(Level level) {
        level_ = level;
    }

    
    void log(Level level, const std::string& msg) {
        if (level > level_) return;

        std::lock_guard<std::mutex> lock(mutex_);
        std::string timestamp = current_time();

        std::ostringstream oss;
        oss << "[" << timestamp << "] "
            << "Rank " << rank_ << " "
            << level_to_string(level) << ": "
            << msg << std::endl;

        if (log_to_file_) {
            file_ << oss.str();
            file_.flush();
        } else {
            std::cout << oss.str();
        }
    }

    
    void error(const std::string& msg)   { log(Level::ERROR, msg); }
    void warning(const std::string& msg) { log(Level::WARNING, msg); }
    void info(const std::string& msg)    { log(Level::INFO, msg); }
    void debug(const std::string& msg)   { log(Level::DEBUG, msg); }

private:
    int rank_ = 0;
    bool log_to_file_ = false;
    std::ofstream file_;
    std::mutex mutex_;
    Level level_ = Level::INFO;

     
    std::string current_time() {
        using namespace std::chrono;
        auto now = system_clock::now();
        std::time_t t = system_clock::to_time_t(now);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
        return std::string(buf);
    }

    
    std::string level_to_string(Level level) {
        switch (level) {
            case Level::ERROR:   return "ERROR";
            case Level::WARNING: return "WARNING";
            case Level::INFO:    return "INFO";
            case Level::DEBUG:   return "DEBUG";
            default:             return "UNKNOWN";
        }
    }
};

#endif  


