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
        database->exec("CREATE TABLE IF NOT EXISTS \"key_exchange_requests\" (\n\t\"id\"\tINTEGER UNIQUE,\n\t\"receiver_login\"\tTEXT,\n\t\"private_key\"\tTEXT,\"receiver_public_key\"\tTEXT,\n\tPRIMARY KEY(\"id\" AUTOINCREMENT));");
        database->exec("CREATE TABLE IF NOT EXISTS \"messages\" (\n\t\"id\"\tINTEGER UNIQUE,\n\t\"sender\"\tTEXT,\n\t\"receiver\"\tTEXT,\n\t\"message\"\tTEXT,\n\t\"conversation_id\"\tINTEGER ,\n\t\"next_public_key\"\tTEXT, \"next_private_key\"\tTEXT, \n\tPRIMARY KEY(\"id\" AUTOINCREMENT)\n);");
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
