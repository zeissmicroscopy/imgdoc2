#pragma once

#include <imgdoc2.h>

#include <utility>
#include "../db/IDbConnection.h"
#include "../db/database_configuration.h"

class Document : public imgdoc2::IDoc, public std::enable_shared_from_this<Document>
{
private:
    std::shared_ptr<IDbConnection> database_connection_;
    std::shared_ptr<DatabaseConfiguration2D> database_configuration_2d_;
public:
    Document(std::shared_ptr<IDbConnection> database_connection, std::shared_ptr<DatabaseConfiguration2D> database_configuration) :
        database_connection_(std::move(database_connection)),
        database_configuration_2d_(std::move(database_configuration))
    {}

    /// Try to get a "write object" for a 2D-document. This method may return an empty shared_ptr
    /// if such an object cannot be constructed.
    /// \returns The writer-object (for 2D-document).
    virtual std::shared_ptr<imgdoc2::IDocWrite2d> GetWriter2d() override;

    /// Try to get a "read object" for a 2D-document. This method may return an empty shared_ptr
    /// if such an object cannot be constructed.
    /// \returns The read-object (for 2D-document).
    virtual std::shared_ptr<imgdoc2::IDocRead2d> GetReader2d() override;

    /*
    /// Try to get a "write object" for a 2D-document. This method may return an empty shared_ptr
    /// if such an object cannot be constructed.
    /// \returns The writer-object (for 3D-document).
    virtual std::shared_ptr<IDbWrite3D> GetWriter3D() = 0;

    /// Try to get a "read object" for a 3D-document. This method may return an empty shared_ptr
    /// if such an object cannot be constructed.
    /// \returns The read-object (for 3D-document).
    virtual std::shared_ptr<IDbRead3D> GetReader3D() = 0;
    */

    virtual ~Document() override = default;
public:
    [[nodiscard]] const std::shared_ptr<IDbConnection>& GetDatabase_connection() const { return this->database_connection_; }
    [[nodiscard]] const std::shared_ptr<DatabaseConfiguration2D>& GetDataBaseConfiguration2d() const { return this->database_configuration_2d_; }
};
