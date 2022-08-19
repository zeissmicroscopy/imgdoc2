#pragma once

#include <string>

class IDbConnection
{
public:

    virtual void Exec(const char* sql_statement) = 0;

    virtual ~IDbConnection() = default;

public:
    void Exec(const std::string& sql_statement)
    {
        this->Exec(sql_statement.c_str());
    }
};