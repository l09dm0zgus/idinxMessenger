//
// Created by cx9ps3 on 21.02.2024.
//

#pragma once
#include <pqxx/pqxx>
namespace db
{
    class Connection
    {
    private:
        std::shared_ptr<pqxx::connection> connection;

    public:
        Connection();
        static std::shared_ptr<Connection> connect();
        std::shared_ptr<pqxx::work> getWork();
    };
}// namespace db
