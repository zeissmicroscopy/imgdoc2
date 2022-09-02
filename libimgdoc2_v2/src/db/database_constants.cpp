#include <stdexcept>
#include "database_constants.h"

using namespace std;

/*static*/const char* DbConstants::kGeneralTable_Name = "GENERAL";

/*static*/const char* DbConstants::kGeneralTable_KeyColumnName = "Key";
/*static*/const char* DbConstants::kGeneralTable_ValueStringColumnName = "ValueString";

/*static*/const char* DbConstants::kTilesInfoTable_DefaultName = "TILESINFO";
/*static*/const char* DbConstants::kTilesDataTable_DefaultName = "TILESDATA";

/*static*/const char* DbConstants::kTilesDataTable_Column_Pk_DefaultName = "Pk";
/*static*/const char* DbConstants::kTilesDataTable_Column_PixelWidth_DefaultName = "PixelWidth";
/*static*/const char* DbConstants::kTilesDataTable_Column_PixelHeight_DefaultName = "PixelHeight";
/*static*/const char* DbConstants::kTilesDataTable_Column_PixelType_DefaultName = "PixelType";
/*static*/const char* DbConstants::kTilesDataTable_Column_TileDataType_DefaultName = "TileDataType";
/*static*/const char* DbConstants::kTilesDataTable_Column_BinDataStorageType_DefaultName = "BinDataStorageType";
/*static*/const char* DbConstants::kTilesDataTable_Column_BinDataId_DefaultName = "BinDataId";

/*static*/const char* DbConstants::kTilesInfoTable_Column_Pk_DefaultName = "Pk";
/*static*/const char* DbConstants::kTilesInfoTable_Column_TileX_DefaultName = "TileX";
/*static*/const char* DbConstants::kTilesInfoTable_Column_TileY_DefaultName = "TileY";
/*static*/const char* DbConstants::kTilesInfoTable_Column_TileW_DefaultName = "TileW";
/*static*/const char* DbConstants::kTilesInfoTable_Column_TileH_DefaultName = "TileH";
/*static*/const char* DbConstants::kTilesInfoTable_Column_PyramidLevel_DefaultName = "PyramidLevel";
/*static*/const char* DbConstants::kTilesInfoTable_Column_TileDataId_DefaultName = "TileDataId";

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
    }

    throw std::invalid_argument("invalid argument for 'item' specified.");
}
