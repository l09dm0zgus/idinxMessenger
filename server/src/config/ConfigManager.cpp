//
// Created by cx9ps3 on 14.02.2024.
//

#include "ConfigManager.hpp"
#include "../utils/Logger.hpp"

config::ConfigManager::ConfigsContainer config::ConfigManager::configs;
std::filesystem::path config::ConfigManager::configsFolder = "configs";

void config::ConfigManager::initialize()
{
#ifdef __unix__
    std::filesystem::path rootPath = "/etc/idinxServer";
    try
    {
        if(!std::filesystem::exists(rootPath))
        {
            std::filesystem::create_directory(rootPath);
        }
        if(!std::filesystem::exists(rootPath / configsFolder))
        {
            std::filesystem::create_directory(rootPath / configsFolder);
        }
        configsFolder = rootPath / configsFolder;
    }
    catch(const std::exception& ex)
    {
        BOOST_LOG_TRIVIAL(error) << ex.what();
    }
#else
    if(!std::filesystem::exists(CONFIGS_FOLDER))
    {
        std::filesystem::create_directory(configsFolder);
    }
#endif
}
