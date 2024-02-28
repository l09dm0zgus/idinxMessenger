//
// Created by cx9ps3 on 21.02.2024.
//

#pragma once
#include "IConfig.hpp"
#include <boost/json.hpp>
#include <filesystem>

namespace config
{
    namespace json = boost::json;
    class DatabaseConfig : public IConfig
    {
    private:
        json::value values;
        std::filesystem::path pathToConfigFile;
        std::string hostname = "localhost";
        int port = 5432;
        std::string userPassword = "your_password";
        std::string userLogin = "postgres";
        std::string dbName = "idinxserverdb";
        void readConfigFile();
        void writeConfigFile();

    public:
        explicit DatabaseConfig(const std::filesystem::path &configDirectory);
        [[nodiscard]] std::string getHostname() const noexcept;
        [[nodiscard]] int getPort() const noexcept;
        [[nodiscard]] std::string getPassword() const noexcept;
        [[nodiscard]] std::string getLogin() const noexcept;
        [[nodiscard]] std::string getName() const noexcept;
        std::string getData() override;
    };
}// namespace config
