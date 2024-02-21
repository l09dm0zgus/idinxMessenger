//
// Created by cx9ps3 on 14.02.2024.
//

#include "ServerConfig.hpp"
#include "../utils/Logger.hpp"
#include <fstream>

config::ServerConfig::ServerConfig(const std::filesystem::path &configDirectory)
{
    pathToConfigFile = std::filesystem::path(configDirectory);
    pathToConfigFile = pathToConfigFile / "server.conf";
    if (std::filesystem::exists(pathToConfigFile))
    {
        readConfigFile();
    }
    else
    {
        writeConfigFile();
    }
}

void config::ServerConfig::readConfigFile()
{
    try
    {
        std::ifstream file(pathToConfigFile);
        values = json::parse(file);
        hostname = values.at("server").at("hostname").as_string().c_str();
        port = values.at("server").at("port").as_int64();
        usedThreads = values.at("server").at("used_threads").as_int64();
    }
    catch (std::exception &ex)
    {
        BOOST_LOG_TRIVIAL(error) << ex.what();
    }
}

void config::ServerConfig::writeConfigFile()
{
    json::object obj;
    obj["server"] = {
            {"hostname", hostname},
            {"port", port},
            {"used_threads", usedThreads}};

    std::ofstream file(pathToConfigFile);
    file << obj;
}

std::string config::ServerConfig::getHostname() const noexcept
{
    return hostname;
}

int config::ServerConfig::getPort() const noexcept
{
    return port;
}

int config::ServerConfig::getUsedThreads() const noexcept
{
    return usedThreads;
}
std::string config::ServerConfig::getData()
{
    return json::value_to<std::string>(values);
}
