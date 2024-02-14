//
// Created by cx9ps3 on 14.02.2024.
//

#pragma once
#include "IConfig.hpp"
#include <boost/json.hpp>
#include <filesystem>

namespace config
{
    namespace json = boost::json;
    class ServerConfig : public IConfig
    {
    private:
        json::value values;
        std::filesystem::path pathToConfigFile;
        std::string hostname = "localhost";
        int port = 1488;
        int usedThreads = -1;
        void readConfigFile();
        void writeConfigFile();
    public:
        explicit ServerConfig(const std::string_view &configDirectory);
        [[nodiscard]] std::string getHostname() const noexcept;
        [[nodiscard]] int getPort() const noexcept;
        [[nodiscard]] int getUsedThreads() const noexcept;
        std::string getData() override;
    };
}
