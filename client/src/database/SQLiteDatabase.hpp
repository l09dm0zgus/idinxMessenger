//
// Created by cx9ps3 on 6/16/24.
//

#pragma once
#include <SQLiteCpp/Database.h>
#include <memory>
#include <string_view>

namespace db
{
    class SQLiteDatabase
    {
    private:
        std::shared_ptr<SQLite::Database> database;
        std::string_view pathToDatabaseFile = {"localDB.db3"};
        void createDatabase();

    public:
        SQLiteDatabase();
        void execute(const std::string_view &query);
        SQLite::Statement query(const std::string_view &query);
    };
}// namespace db
