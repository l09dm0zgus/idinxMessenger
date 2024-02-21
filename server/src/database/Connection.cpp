//
// Created by cx9ps3 on 21.02.2024.
//

#include "Connection.hpp"
#include "../config/ConfigManager.hpp"
#include "../config/DatabaseConfig.hpp"
#include <sstream>

db::Connection::Connection()
{
    std::stringstream ss;
    ss << "host=" << config::ConfigManager::getConfig<config::DatabaseConfig>()->getHostname();
    ss << " port=" << config::ConfigManager::getConfig<config::DatabaseConfig>()->getPort();
    ss << " dbname=" << config::ConfigManager::getConfig<config::DatabaseConfig>()->getName();
    ss << " user=" << config::ConfigManager::getConfig<config::DatabaseConfig>()->getLogin();
    ss << " password=" << config::ConfigManager::getConfig<config::DatabaseConfig>()->getPassword();
    BOOST_LOG_TRIVIAL(info) << "Trying connect to PostgreSQL server with host: " << config::ConfigManager::getConfig<config::DatabaseConfig>()->getHostname() << " and port: " << config::ConfigManager::getConfig<config::DatabaseConfig>()->getPort();

    try
    {
        connection = std::make_shared<pqxx::connection>(ss.str());
    }
    catch (const std::exception &ex)
    {
        BOOST_LOG_TRIVIAL(error) << "Exception: " << ex.what();
    }
}

std::shared_ptr<db::Connection> db::Connection::connect()
{
    return std::make_shared<Connection>();
}

std::shared_ptr<pqxx::work> db::Connection::getWork()
{
    return std::make_shared<pqxx::work>(*connection);
}
