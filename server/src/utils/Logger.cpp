//
// Created by cx9ps3 on 09.02.2024.
//

#include "Logger.hpp"
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <filesystem>

void utils::Logger::initialize()
{

    if (!std::filesystem::exists("logs"))
    {
        std::filesystem::create_directory("logs");
    }
    logging::add_console_log(std::clog, keywords::format = FORMAT);
    logging::add_file_log(
            keywords::auto_flush = true,
            keywords::file_name = FILE_NAME,
            keywords::rotation_size = 10 * 1024 * 1024,
            keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
            keywords::format = FORMAT);

    logging::core::get()->set_filter(
            logging::trivial::severity >= logging::trivial::trace);

    utils::logging::add_common_attributes();
}
