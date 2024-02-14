//
// Created by cx9ps3 on 09.02.2024.
//

#pragma once
#include <boost/assert.hpp>
#include <boost/log/trivial.hpp>
#include <sstream>

namespace utils
{
    namespace logging = boost::log;
    namespace src = boost::log::sources;
    namespace sinks = boost::log::sinks;
    namespace keywords = boost::log::keywords;


    class Logger
    {
    private:
        static constexpr std::string_view FILE_NAME = "logs/idinxServer_%N_%Y-%m-%d_%H-%M-%S.log";
        static constexpr std::string_view FORMAT = "[%TimeStamp%] - PID = %ProcessID% - TID = %ThreadID% : [%Severity%] %Message%";

    public:
        static void initialize();
    };

#define ASSERT_LOG(expr, msg) \
    if (!expr) { BOOST_LOG_TRIVIAL(fatal) << "Assertion :" << #expr << " failed. Message: " << msg; }
}// namespace utils
