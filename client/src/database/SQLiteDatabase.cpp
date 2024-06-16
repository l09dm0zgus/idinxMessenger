//
// Created by cx9ps3 on 6/16/24.
//

#include "SQLiteDatabase.hpp"
#include <iostream>
void db::SQLiteDatabase::createDatabase()
{
    try
    {
        database = std::make_shared<SQLite::Database>(pathToDatabaseFile, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        database->exec("CREATE TABLE IF NOT EXISTS \"users\" ( \"id\" INTEGER UNIQUE,\"login\" TEXT,\"password\"	TEXT,PRIMARY KEY(\"id\" AUTOINCREMENT));");
    }
    catch (SQLite::Exception &ex)
    {
        std::cout << "Error: " << ex.what() << "\n";
    }
}

void db::SQLiteDatabase::execute(const std::string_view &query)
{
    database->exec(query.data());
}

SQLite::Statement db::SQLiteDatabase::query(const std::string_view &query)
{
    return {*database, query.data()};
}

db::SQLiteDatabase::SQLiteDatabase()
{
    createDatabase();
}