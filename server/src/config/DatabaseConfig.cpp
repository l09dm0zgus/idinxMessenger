//
// Created by cx9ps3 on 21.02.2024.
//

#include "DatabaseConfig.hpp"
#include "../utils/Logger.hpp"
#include <fstream>

void config::DatabaseConfig::writeConfigFile()
{
    json::object obj;
    obj["database"] = {
            {"hostname", hostname},
            {"port", port},
            {"database_name", dbName},
            {"user_password", userPassword},
            {"user_login", userLogin}};

    std::ofstream file(pathToConfigFile);
    file << obj;
}

void config::DatabaseConfig::readConfigFile()
{
    try
    {
        std::ifstream file(pathToConfigFile);
        values = json::parse(file);
        hostname = values.at("database").at("hostname").as_string().c_str();
        port = values.at("database").at("port").as_int64();
        dbName = values.at("database").at("database_name").as_string().c_str();
        userPassword = values.at("database").at("user_password").as_string().c_str();
        userLogin = values.at("database").at("user_login").as_string().c_str();
    }
    catch (std::exception &ex)
    {
        BOOST_LOG_TRIVIAL(error) << ex.what();
    }
}

config::DatabaseConfig::DatabaseConfig(const std::filesystem::path &configDirectory)
{
    pathToConfigFile = std::filesystem::path(configDirectory);
    pathToConfigFile = pathToConfigFile / "database.conf";
    if (std::filesystem::exists(pathToConfigFile))
    {
        readConfigFile();
    }
    else
    {
        writeConfigFile();
    }
}

std::string config::DatabaseConfig::getHostname() const noexcept
{
    return hostname;
}

int config::DatabaseConfig::getPort() const noexcept
{
    return port;
}

std::string config::DatabaseConfig::getPassword() const noexcept
{
    return userPassword;
}

std::string config::DatabaseConfig::getLogin() const noexcept
{
    return userLogin;
}

std::string config::DatabaseConfig::getName() const noexcept
{
    return dbName;
}

std::string config::DatabaseConfig::getData()
{
    return json::value_to<std::string>(values);
}
