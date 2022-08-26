#pragma once

#include <memory>
#include <string>
#include <cstdint>

class IDbStatement
{
public:
    virtual void Reset() = 0;

    virtual void BindInt32(int index, std::int32_t value) = 0;
    virtual void BindInt64(int index, std::int64_t value) = 0;
    virtual void BindDouble(int index, double value) = 0;

    virtual int GetResultInt32(int column) = 0;
    virtual std::int64_t GetResultInt64(int column) = 0;
    virtual double GetResultDouble(int column) = 0;
};

class IDbConnection
{
public:

    virtual void Exec(const char* sql_statement) = 0;

    virtual std::int64_t ExecuteAndGetLastRowId(IDbStatement* statement) = 0;

    virtual std::unique_ptr<IDbStatement> PrepareStatement(const std::string& sql_statement) = 0;

    virtual bool StepStatement(IDbStatement* statement) = 0;

    virtual ~IDbConnection() = default;

public:
    void Exec(const std::string& sql_statement)
    {
        this->Exec(sql_statement.c_str());
    }
};
