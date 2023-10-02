#ifndef CFGPARSER_HPP_
#define CFGPARSER_HPP_

#include "Error.hpp"
#include <fstream>
#include <unordered_map>
#include <string>
#include <any>
#include <iostream>
#include <algorithm>
#include <filesystem>

namespace utils
{
    static const std::string getCurrDir() {
#ifdef _WIN32
        wchar_t buffer[MAX_PATH];
        GetModuleFileNameW(NULL, buffer, MAX_PATH);
        std::wstringstream wss;
        wss << buffer;
        std::wstring wstr = wss.str();
        std::filesystem::path executablePath(wstr);
#else
        std::filesystem::path executablePath = std::filesystem::path(
        std::getenv("PWD")) / std::filesystem::read_symlink("/proc/self/exe");
#endif /* !_WIN32 */
        return (executablePath.parent_path().string());
    }

    class ParseCFG
    {
        public:
            ParseCFG() = delete;
            ParseCFG(const std::string &filepath)
            {
                try {
                    _cfgFile.open(filepath);
                    if (!_cfgFile)
                        throw Error("Failed to open " + filepath, "CFGParser");
                    std::string line;
                    while (std::getline(_cfgFile, line)) {
                        if (line[0] != '#' && !line.empty()) {
                            line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
                            std::size_t delimNdx = line.find("=");
                            if (delimNdx != 0 && delimNdx + 1 != line.size())
                                _cfgKeys.insert({line.substr(0, delimNdx), line.substr(delimNdx + 1)});
                        }
                    }
                } catch (const Error &e) {
                    std::cerr << e.what() << std::endl;
                }
            };

            ~ParseCFG()
            {
                if (_cfgFile.is_open())
                    _cfgFile.close();
            };

            template<typename T>
            T getData(const std::string &key)
            {
                if (_cfgKeys.empty())
                    throw Error("No data available to retrieve.");
                try {
                    return std::any_cast<T>(_cfgKeys.at(key));
                }
                catch (const std::out_of_range &) {
                    throw Error("Key not found: " + key);
                }
                catch (const std::bad_any_cast &) {
                    throw Error("Type mismatch for key " + key);
                }
            }

        protected:
        private:
            std::ifstream _cfgFile;
            std::unordered_map<std::string, std::any> _cfgKeys;
    };
}

#endif /* !CFGPARSER_HPP_ */