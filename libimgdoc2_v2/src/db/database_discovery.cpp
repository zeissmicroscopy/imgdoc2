#include <algorithm>
#include <vector>
#include <sstream>
#include "database_discovery.h"
#include "database_constants.h"
#include "IDbConnection.h"
#include <exceptions.h>

using namespace std;
using namespace imgdoc2;

void DbDiscovery::DoDiscovery()
{
    // first step - find the "GENERAL" table and see if we can make sense of it
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

    // ok, so now get the content for key=TilesInfoTable and key=TilesDataTable
    ostringstream ss;
    ss << "SELECT [" << DbConstants::kGeneralTable_ValueStringColumnName << "] FROM [" << DbConstants::kGeneralTable_Name << "] WHERE [" << DbConstants::kGeneralTable_KeyColumnName << "]='" << "TilesInfoTable" << "';";
    auto statement = this->db_connection_->PrepareStatement(ss.str());
    if (!this->db_connection_->StepStatement(statement.get()))
    {
        // ok, if the "TilesInfoTable"-item is not present, in the property-bag, then we fall back to "default name"
        general_discovery_result.tileinfotable_name = DbConstants::kTilesInfoTable_DefaultName;    
    }
    else
    {
        general_discovery_result.tileinfotable_name = statement->GetResultString(1);
    }


    ss = ostringstream();
    ss << "SELECT [" << DbConstants::kGeneralTable_ValueStringColumnName << "] FROM [" << DbConstants::kGeneralTable_Name << "] WHERE [" << DbConstants::kGeneralTable_KeyColumnName << "]='" << "TilesDataTable" << "';";
    statement = this->db_connection_->PrepareStatement(ss.str());
    if (!this->db_connection_->StepStatement(statement.get()))
    {
        // ok, if the "TilesInfoTable"-item is not present, in the property-bag, then we fall back to "default name"
        general_discovery_result.tilesdatatable_name= DbConstants::kTilesDataTable_DefaultName;
    }
    else
    {
        general_discovery_result.tilesdatatable_name = statement->GetResultString(1);
    }

    return general_discovery_result;
}