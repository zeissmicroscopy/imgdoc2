#pragma once

#include <imgdoc2.h>
#include "../db/IDbConnection.h"

class Document : public imgdoc2::IDoc
{
private:
    std::shared_ptr<IDbConnection> database_connection_;
public:
    Document(std::shared_ptr<IDbConnection> database_connection) :
        database_connection_(database_connection)
    {}

    virtual ~Document() = default;
};