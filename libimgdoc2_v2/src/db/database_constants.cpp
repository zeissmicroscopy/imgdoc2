#include <stdexcept>
#include "database_constants.h"

using namespace std;

/*static*/const char* const DbConstants::kGeneralTable_Name = "GENERAL";

/*static*/const char* const DbConstants::kGeneralTable_KeyColumnName = "Key";
/*static*/const char* const DbConstants::kGeneralTable_ValueStringColumnName = "ValueString";

/*static*/const char* const DbConstants::kTilesInfoTable_DefaultName = "TILESINFO";
/*static*/const char* const DbConstants::kTilesDataTable_DefaultName = "TILESDATA";
/*static*/const char* const DbConstants::kTilesSpatialIndexTable_DefaultName = "TILESSPATIALINDEX";

/*static*/const char* const DbConstants::kTilesDataTable_Column_Pk_DefaultName = "Pk";
/*static*/const char* const DbConstants::kTilesDataTable_Column_PixelWidth_DefaultName = "PixelWidth";
/*static*/const char* const DbConstants::kTilesDataTable_Column_PixelHeight_DefaultName = "PixelHeight";
/*static*/const char* const DbConstants::kTilesDataTable_Column_PixelType_DefaultName = "PixelType";
/*static*/const char* const DbConstants::kTilesDataTable_Column_TileDataType_DefaultName = "TileDataType";
/*static*/const char* const DbConstants::kTilesDataTable_Column_BinDataStorageType_DefaultName = "BinDataStorageType";
/*static*/const char* const DbConstants::kTilesDataTable_Column_BinDataId_DefaultName = "BinDataId";

/*static*/const char* const DbConstants::kTilesInfoTable_Column_Pk_DefaultName = "Pk";
/*static*/const char* const DbConstants::kTilesInfoTable_Column_TileX_DefaultName = "TileX";
/*static*/const char* const DbConstants::kTilesInfoTable_Column_TileY_DefaultName = "TileY";
/*static*/const char* const DbConstants::kTilesInfoTable_Column_TileW_DefaultName = "TileW";
/*static*/const char* const DbConstants::kTilesInfoTable_Column_TileH_DefaultName = "TileH";
/*static*/const char* const DbConstants::kTilesInfoTable_Column_PyramidLevel_DefaultName = "PyramidLevel";
/*static*/const char* const DbConstants::kTilesInfoTable_Column_TileDataId_DefaultName = "TileDataId";

/*static*/const char* const DbConstants::kDimensionColumnPrefix_Default = "Dim_";
/*static*/const char* const DbConstants::kIndexForDimensionColumnPrefix_Default = "IndexForDim_";

/*static*/const char* DbConstants::GetGeneralTable_ItemKey(GeneralTableItems item)
{
    switch (item)
    {
    case GeneralTableItems::kVersion:
        return "Version";
    case GeneralTableItems::kTilesDataTable:
        return "TilesDataTable";
    case GeneralTableItems::kTilesInfoTable:
        return "TilesInfoTable";
    case GeneralTableItems::kDocType:
        return "DocType";
    case GeneralTableItems::kBlobTable:
        return "BlobTable";
    case GeneralTableItems::kSpatialIndexTable:
        return "SpatialIndexTable";
    }

    throw std::invalid_argument("invalid argument for 'item' specified.");
}
