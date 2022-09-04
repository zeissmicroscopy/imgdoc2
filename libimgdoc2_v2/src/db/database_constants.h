#pragma once

/// Values that represent "well known items" in the 'GENERAL'-table.
enum class GeneralTableItems
{
    kVersion,
    kTilesDataTable,    ///< An enum constant representing "Name of the 'TILESDATA'-table.
    kTilesInfoTable,
    kDocType,
    kBlobTable          ///< An enum constant representing "Name of the 'BLOB'-table.
};

class DbConstants
{
public:
    static const char* const kGeneralTable_Name;                  // = "GENERAL"

    static const char* const kGeneralTable_KeyColumnName;         // = "Key"
    static const char* const kGeneralTable_ValueStringColumnName; // = "ValueString"

    static const char* const kTilesInfoTable_DefaultName;         // = "TILESINFO"
    static const char* const kTilesDataTable_DefaultName;         // = "TILESDATA"

    static const char* const kTilesDataTable_Column_Pk_DefaultName;
    static const char* const kTilesDataTable_Column_PixelWidth_DefaultName;
    static const char* const kTilesDataTable_Column_PixelHeight_DefaultName;
    static const char* const kTilesDataTable_Column_PixelType_DefaultName;
    static const char* const kTilesDataTable_Column_TileDataType_DefaultName;
    static const char* const kTilesDataTable_Column_BinDataStorageType_DefaultName;
    static const char* const kTilesDataTable_Column_BinDataId_DefaultName;


    static const char* const kTilesInfoTable_Column_Pk_DefaultName;
    static const char* const kTilesInfoTable_Column_TileX_DefaultName;
    static const char* const kTilesInfoTable_Column_TileY_DefaultName;
    static const char* const kTilesInfoTable_Column_TileW_DefaultName;
    static const char* const kTilesInfoTable_Column_TileH_DefaultName;
    static const char* const kTilesInfoTable_Column_PyramidLevel_DefaultName;
    static const char* const kTilesInfoTable_Column_TileDataId_DefaultName;

    static const char* const kDimensionColumnPrefix_Default;  // = "Dim_"
    static const char* const kIndexForDimensionColumnPrefix_Default; // = "IndexForDim_"


    static const char* GetGeneralTable_ItemKey(GeneralTableItems item);
};

