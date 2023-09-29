#ifndef LOGS_HPP_
#define LOGS_HPP_

#include <string>
#include <fstream>
#include <iostream>
#include <ctime>

#define INFO  "I"
#define WARN  "W"
#define ERR   "E"

namespace net
{
    class Log
    {
        public:
            Log() = delete;
            Log(const std::string &filepath)
            {
                _logdate = "";
                _logfile = std::ofstream(filepath);
            };
            ~Log() = default;

            void logTo(const std::string &status, const std::string &msg)
            {
                std::time_t now = std::time(nullptr);
                std::string mbstr;
                char tm[100];
                std::strftime(tm, sizeof(tm), "%D %X", std::localtime(&now));
                _logdate = tm;
                if (status == INFO)
                    std::cout << _logdate << " | " << status << " | " << msg << std::endl;
                else
                    std::cerr << _logdate << " | " << status << " | " << msg << std::endl;
                _logfile << _logdate << " | " << status << " | " << msg << std::endl;
            };

        private:
            std::string _logdate;
            std::ofstream _logfile;
    };
}

#endif /* !LOGS_HPP_ */