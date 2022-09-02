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

    // now, check whether those tables exists and are useable
    this->Check_Tables(general_table_discovery_result);
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
        DbConstants::GetGeneralTable_ItemKey(GeneralTableItems::kDocType),  // "DocType",
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
        DbConstants::GetGeneralTable_ItemKey(GeneralTableItems::kTilesInfoTable), //  "TilesInfoTable",
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
        DbConstants::GetGeneralTable_ItemKey(GeneralTableItems::kTilesDataTable), // "TilesDataTable",
        &str))
    {
        general_discovery_result.tilesdatatable_name = str;
    }
    else
    {
        general_discovery_result.tilesdatatable_name = DbConstants::kTilesDataTable_DefaultName;
    }

    if (Utilities::TryReadStringFromPropertyBag(
        this->db_connection_.get(),
        DbConstants::kGeneralTable_Name,
        DbConstants::kGeneralTable_KeyColumnName,
        DbConstants::kGeneralTable_ValueStringColumnName,
        DbConstants::GetGeneralTable_ItemKey(GeneralTableItems::kBlobTable), //"BlobTable",
        &str))
    {
        general_discovery_result.blobtable_name = str;
    }
   
    return general_discovery_result;
}

void DbDiscovery::Check_Tables(const GeneralDataDiscoveryResult& general_table_discovery_result)
{
    vector<ExpectedColumnsInfo> expected_row_for_table
    {
        ExpectedColumnsInfo(DbConstants::kTilesDataTable_Column_Pk_DefaultName),
        ExpectedColumnsInfo(DbConstants::kTilesDataTable_Column_PixelWidth_DefaultName),
        ExpectedColumnsInfo(DbConstants::kTilesDataTable_Column_PixelHeight_DefaultName),
        ExpectedColumnsInfo(DbConstants::kTilesDataTable_Column_PixelType_DefaultName),
        ExpectedColumnsInfo(DbConstants::kTilesDataTable_Column_TileDataType_DefaultName),
        ExpectedColumnsInfo(DbConstants::kTilesDataTable_Column_BinDataStorageType_DefaultName),
        ExpectedColumnsInfo(DbConstants::kTilesDataTable_Column_BinDataId_DefaultName),
    };

    auto columns_of_table = this->db_connection_->GetTableInfo(general_table_discovery_result.tilesdatatable_name.c_str());

    for (const auto& i : expected_row_for_table)
    {
        if (!any_of(
            columns_of_table.cbegin(), 
            columns_of_table.cend(), 
            [&](const IDbConnection::ColumnInfo& column_info)->bool
            {
                return column_info.column_name == i.column_name;
            }))
        {
            throw discovery_exception("Column not found or column is inappropriate.");
        }
    }

    expected_row_for_table = vector<ExpectedColumnsInfo>
    {
        ExpectedColumnsInfo(DbConstants::kTilesInfoTable_Column_Pk_DefaultName),
        ExpectedColumnsInfo(DbConstants::kTilesInfoTable_Column_TileX_DefaultName),
        ExpectedColumnsInfo(DbConstants::kTilesInfoTable_Column_TileY_DefaultName),
        ExpectedColumnsInfo(DbConstants::kTilesInfoTable_Column_TileW_DefaultName),
        ExpectedColumnsInfo(DbConstants::kTilesInfoTable_Column_TileH_DefaultName),
        ExpectedColumnsInfo(DbConstants::kTilesInfoTable_Column_PyramidLevel_DefaultName),
        ExpectedColumnsInfo(DbConstants::kTilesInfoTable_Column_TileDataId_DefaultName),
    };

    columns_of_table = this->db_connection_->GetTableInfo(general_table_discovery_result.tileinfotable_name.c_str());

    for (const auto& i : expected_row_for_table)
    {
        if (!any_of(
            columns_of_table.cbegin(),
            columns_of_table.cend(),
            [&](const IDbConnection::ColumnInfo& column_info)->bool
            {
                return column_info.column_name == i.column_name;
            }))
        {
            throw discovery_exception("Column not found or column is inappropriate.");
        }
    }
}