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
        std::filesystem::path executablePath = std::filesystem::path(
        std::getenv("PWD")) / std::filesystem::read_symlink("/proc/self/exe");
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
            const T getData(const std::string &key)
            {
                if (_cfgKeys.empty())
                    throw Error("No data available to retrieve.");
                try {
                    return (std::any_cast<T>(_cfgKeys[key]));
                } catch (const std::out_of_range &e) {
                    throw Error("Failed to obtain " + key);
                } catch (const std::bad_any_cast &e) {
                    throw Error("Failed to convert to asked type");
                }
            }

        protected:
        private:
            std::ifstream _cfgFile;
            std::unordered_map<std::string, std::any> _cfgKeys;
    };
}

#endif /* !CFGPARSER_HPP_ */