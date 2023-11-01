#ifndef LOGS_HPP_
#define LOGS_HPP_

#include <string>
#include <fstream>
#include <iostream>
#include <ctime>

static constexpr std::string_view logInfo = "Server Thread/Info";
static constexpr std::string_view logWarn = "Server Thread/Warn";
static constexpr std::string_view logErr = "Server Thread/Error";
static constexpr std::string_view logGameInfo = "Game Thread/Info  ";
static constexpr std::string_view logGameWarn = "Game Thread/Warn  ";
static constexpr std::string_view logGameErr = "Game Thread/Error  ";

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

#ifdef _WIN32
                struct tm localTime;
                if (localtime_s(&localTime, &now) != 0)
                {
                    std::cerr << "Error getting local time." << std::endl;
                    return;
                }
                if (std::strftime(tm, sizeof(tm), "%D %X", &localTime) == 0)
                {
                    std::cerr << "Error formatting time." << std::endl;
                    return;
                }
#else
                if (std::strftime(tm, sizeof(tm), "%D %X", std::localtime(&now)) == 0)
                {
                    std::cerr << "Error formatting time." << std::endl;
                    return;
                }
#endif

                _logdate = tm;
                if (status == logInfo)
                    std::cout << _logdate << " | " << status << " | " << msg << std::endl;
                else
                    std::cerr << _logdate << " | " << status << " | " << msg << std::endl;
                _logfile << _logdate << " | " << status << " | " << msg << std::endl;
            }

        private:
            std::string _logdate;
            std::ofstream _logfile;
    };
}

#endif /* !LOGS_HPP_ */