//
// Created by cx9ps3 on 14.02.2024.
//
#pragma once
#include <map>
#include <typeinfo>
#include <memory>
#include <filesystem>
#include "IConfig.hpp"
#include "../utils/Logger.hpp"

namespace config
{
    class ConfigManager
    {
    private:
        using ConfigsContainer = std::unordered_map<size_t ,std::shared_ptr<IConfig>>;
        static ConfigsContainer configs;
        static std::filesystem::path configsFolder;
    public:
        static void initialize();
        template<class T>
        static void addConfig()
        {
            configs[typeid(T).hash_code()] = std::make_shared<T>(configsFolder);
        }

        template<class T>
        static std::shared_ptr<T> getConfig()
        {
            ASSERT_LOG(configs.count(typeid(T).hash_code()), "Config does not exists!");
            return std::dynamic_pointer_cast<T>(configs[typeid(T).hash_code()]);
        }
    };
}
