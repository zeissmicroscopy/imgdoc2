#include <algorithm>
#include <vector>
#include <sstream>
#include <cstring>
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
    this->Check_Tables_And_Determine_Dimensions(general_table_discovery_result);

    auto configuration2d = make_shared<DatabaseConfiguration2D>();
    this->FillInformationForConfiguration2D(general_table_discovery_result, *configuration2d);
    this->configuration_ = configuration2d;
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

void DbDiscovery::FillInformationForConfiguration2D(const GeneralDataDiscoveryResult& general_data_discovery_result, DatabaseConfiguration2D& database_configuration_2d)
{
    database_configuration_2d.SetDimensionColumnPrefix(DbConstants::kDimensionColumnPrefix_Default/*"Dim_"*/);
    database_configuration_2d.SetIndexForDimensionColumnPrefix(DbConstants::kIndexForDimensionColumnPrefix_Default/*"IndexForDim_"*/);
    database_configuration_2d.SetTableName(DatabaseConfigurationCommon::TableTypeCommon::GeneralInfo, DbConstants::kGeneralTable_Name/*"GENERAL"*/);
    database_configuration_2d.SetTableName(DatabaseConfigurationCommon::TableTypeCommon::TilesData, general_data_discovery_result.tilesdatatable_name.c_str());
    database_configuration_2d.SetTableName(DatabaseConfigurationCommon::TableTypeCommon::TilesInfo, general_data_discovery_result.tileinfotable_name.c_str());

    database_configuration_2d.SetDefaultColumnNamesForTilesInfoTable();
    database_configuration_2d.SetTileDimensions(general_data_discovery_result.dimensions.cbegin(), general_data_discovery_result.dimensions.cend());
    database_configuration_2d.SetIndexedTileDimensions(general_data_discovery_result.indexed_dimensions.cbegin(), general_data_discovery_result.indexed_dimensions.cend());
    database_configuration_2d.SetDefaultColumnNamesForTilesDataTable();
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

void DbDiscovery::Check_Tables_And_Determine_Dimensions(GeneralDataDiscoveryResult& general_table_discovery_result)
{
    vector<ExpectedColumnsInfo> expected_columns_for_table
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

    for (const auto& expected_column_info : expected_columns_for_table)
    {
        if (!any_of(
            columns_of_table.cbegin(),
            columns_of_table.cend(),
            [&](const IDbConnection::ColumnInfo& column_info)->bool
            {
                return column_info.column_name == expected_column_info.column_name;
            }))
        {
            throw discovery_exception("Column not found or column is inappropriate.");
        }
    }

    expected_columns_for_table = vector<ExpectedColumnsInfo>
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

    for (const auto& expected_column_info : expected_columns_for_table)
    {
        if (!any_of(
            columns_of_table.cbegin(),
            columns_of_table.cend(),
            [&](const IDbConnection::ColumnInfo& column_info)->bool
            {
                return column_info.column_name == expected_column_info.column_name;
            }))
        {
            throw discovery_exception("Column not found or column is inappropriate.");
        }
    }

    // now we look for columns where the name is starting with "Dim_" - this gives us the list of dimensions
    const size_t length_of_column_prefix_string = strlen(DbConstants::kDimensionColumnPrefix_Default);
    for (const auto& column : columns_of_table)
    {
        if (column.column_name.find(DbConstants::kDimensionColumnPrefix_Default) == 0 && column.column_name.length() == length_of_column_prefix_string + 1)
        {
            general_table_discovery_result.dimensions.push_back(column.column_name[length_of_column_prefix_string]);
        }
    }

    // Ok, and now we find out which of the dimensions are indexed.
    const auto list_of_indices = this->db_connection_->GetIndicesOfTable(general_table_discovery_result.tileinfotable_name.c_str());
    const size_t length_of_dimension_index = strlen(DbConstants::kIndexForDimensionColumnPrefix_Default);
    for (const auto& index_column : list_of_indices)
    {
        if (index_column.index_name.find(DbConstants::kIndexForDimensionColumnPrefix_Default) == 0 && index_column.index_name.length() == length_of_dimension_index + 1)
        {
            // we better make sure that the indices we get here are actually listed as 'dimensions'
            // TODO(JBL): maybe we should report some warning (if this is not the case)
            Dimension dimension_of_index = index_column.index_name[length_of_dimension_index];
            if (any_of(general_table_discovery_result.dimensions.cbegin(), general_table_discovery_result.dimensions.cend(), [dimension_of_index](Dimension dimension)->bool {return dimension == dimension_of_index; }))
            {
                general_table_discovery_result.indexed_dimensions.push_back(index_column.index_name[length_of_dimension_index]);
            }
        }
    }
}