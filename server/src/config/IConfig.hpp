//
// Created by cx9ps3 on 14.02.2024.
//
#pragma once
#include <string>
namespace config
{
    class IConfig
    {
    public:
        IConfig() = default;
        ~IConfig() = default;
        IConfig(IConfig&&) = default;
        IConfig(const IConfig&) = default;
        IConfig& operator=(IConfig&&) = default;
        IConfig& operator=(const IConfig&) = default;
        virtual std::string getData() = 0;
    };
}
