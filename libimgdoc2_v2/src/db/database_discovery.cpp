#include <algorithm>
#include <vector>
#include <sstream>
#include "database_discovery.h"
#include "database_constants.h"
#include "IDbConnection.h"
#include <exceptions.h>

#include "utilities.h"

using namespace std;
using namespace imgdoc2;

void DbDiscovery::DoDiscovery()
{
    // first step - find the "GENERAL" table and see if we can make sense of it
    GeneralDataDiscoveryResult general_table_discovery_result = this->DiscoverGeneralTable();
}

std::shared_ptr<DatabaseConfigurationCommon> DbDiscovery::GetDatabaseConfiguration()
{
    auto configuration = this->configuration_;
    if (!configuration)
    {
        throw logic_error("No valid database-configuration.");
    }

    return configuration;
}

DbDiscovery::GeneralDataDiscoveryResult DbDiscovery::DiscoverGeneralTable()
{
    auto columns_of_general_table = this->db_connection_->GetTableInfo(DbConstants::kGeneralTable_Name);

    // ok, if that worked, then we need to have two columns named "Key" and "ValueString"
    if (!
        (any_of(columns_of_general_table.cbegin(), columns_of_general_table.cend(), [](const IDbConnection::ColumnInfo& column_info)->bool {return column_info.column_name == DbConstants::kGeneralTable_KeyColumnName; }) &&
            any_of(columns_of_general_table.cbegin(), columns_of_general_table.cend(), [](const IDbConnection::ColumnInfo& column_info)->bool {return column_info.column_name == DbConstants::kGeneralTable_ValueStringColumnName; }))
        )
    {
        throw discovery_exception("Unexpected content in the 'GENERAL'-table");
    }

    GeneralDataDiscoveryResult general_discovery_result;

    // first of all, read the key=DocType
    string str;
    if (!Utilities::TryReadStringFromPropertyBag(
        this->db_connection_.get(),
        DbConstants::kGeneralTable_Name,
        DbConstants::kGeneralTable_KeyColumnName,
        DbConstants::kGeneralTable_ValueStringColumnName,
        "DocType",
        &str))
    {
        throw discovery_exception("Property 'DocType' not present, refusing to open this database.");
    }

    if (str != "Tiles2D")
    {
        throw discovery_exception("Only 'DocType' = \"Tiles2D\" is supported at this time.");
    }

    general_discovery_result.document_type = DocumentType::Image2d;

    // ok, so now get the content for key=TilesInfoTable and key=TilesDataTable. Those will give us the name of the tables we are
    // to use. If the values are not present, we go with default-values.
    if (Utilities::TryReadStringFromPropertyBag(
        this->db_connection_.get(), 
        DbConstants::kGeneralTable_Name, 
        DbConstants::kGeneralTable_KeyColumnName, 
        DbConstants::kGeneralTable_ValueStringColumnName,
        "TilesInfoTable",
        &str))
    {
        general_discovery_result.tileinfotable_name = str;
    }
    else
    {
        general_discovery_result.tileinfotable_name = DbConstants::kTilesInfoTable_DefaultName;
    }

    if (Utilities::TryReadStringFromPropertyBag(
        this->db_connection_.get(),
        DbConstants::kGeneralTable_Name,
        DbConstants::kGeneralTable_KeyColumnName,
        DbConstants::kGeneralTable_ValueStringColumnName,
        "TilesDataTable",
        &str))
    {
        general_discovery_result.tilesdatatable_name = str;
    }
    else
    {
        general_discovery_result.tilesdatatable_name = DbConstants::kTilesDataTable_DefaultName;
    }

    //ostringstream ss;
    //ss << "SELECT [" << DbConstants::kGeneralTable_ValueStringColumnName << "] FROM [" << DbConstants::kGeneralTable_Name << "] WHERE [" << DbConstants::kGeneralTable_KeyColumnName << "]='" << "TilesInfoTable" << "';";
    //auto statement = this->db_connection_->PrepareStatement(ss.str());
    //if (!this->db_connection_->StepStatement(statement.get()))
    //{
    //    // ok, if the "TilesInfoTable"-item is not present, in the property-bag, then we fall back to "default name"
    //    general_discovery_result.tileinfotable_name = DbConstants::kTilesInfoTable_DefaultName;    
    //}
    //else
    //{
    //    general_discovery_result.tileinfotable_name = statement->GetResultString(0);
    //}


    //ss = ostringstream();
    //ss << "SELECT [" << DbConstants::kGeneralTable_ValueStringColumnName << "] FROM [" << DbConstants::kGeneralTable_Name << "] WHERE [" << DbConstants::kGeneralTable_KeyColumnName << "]='" << "TilesDataTable" << "';";
    //statement = this->db_connection_->PrepareStatement(ss.str());
    //if (!this->db_connection_->StepStatement(statement.get()))
    //{
    //    // ok, if the "TilesDataTable"-item is not present, in the property-bag, then we fall back to "default name"
    //    general_discovery_result.tilesdatatable_name= DbConstants::kTilesDataTable_DefaultName;
    //}
    //else
    //{
    //    general_discovery_result.tilesdatatable_name = statement->GetResultString(0);
    //}



    return general_discovery_result;
}