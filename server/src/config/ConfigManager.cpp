//
// Created by cx9ps3 on 14.02.2024.
//

#include "ConfigManager.hpp"
#include <filesystem>

config::ConfigManager::ConfigsContainer config::ConfigManager::configs;

void config::ConfigManager::initialize()
{
    if(!std::filesystem::exists(CONFIGS_FOLDER))
    {
        std::filesystem::create_directory(CONFIGS_FOLDER);
    }
}
