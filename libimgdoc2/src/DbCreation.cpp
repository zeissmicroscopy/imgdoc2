#include "pch.h"

#include <SQLiteCpp/Database.h>
#include <sqlite3.h> // for SQLITE_ERROR and SQLITE_VERSION_NUMBER
#include <sstream>
#include <iostream>

#include "DbCreation.h"
#include "../inc/Interface.h"
#include "DbBase.h"
#include "DbWrite.h"
#include "DbRead.h"
#include "DbDiscover.h"
#include "CCustomQueries.h"
#include "DbGlobalInfo3D.h"
#include "DbCreation3D.h"
#include "DbMasterInfoTable.h"


using namespace SlImgDoc;
using namespace std;

/*
https://www.tutorialspoint.com/sqlite/sqlite_constraints.htm
*/

#if false
static void CreateTileTable(SQLite::Database* db)
{
    stringstream ss;
    ss << "CREATE TABLE[" << CDbBase::TableName_DocumentInfo << "]("
        "[" << CDbBase::DocumentInfoColName_Version << "] INTEGER(8),"
        "[" << CDbBase::DocumentInfoColName_CoordinateDimensions << "]   TEXT,"
        "[" << CDbBase::DocumentInfoColName_IndexedDimensions << "] TEXT"
        ");";
    db->exec(ss.str());

    ss = stringstream();
    ss << "CREATE TABLE[" << CDbBase::TableName_TileTable << "]("
        "[PK] INTEGER NOT NULL PRIMARY KEY,"
        "[C] INTEGER(4),"
        "[T] INTEGER(4),"
        "[Z] INTEGER(4),"
        "[M] INTEGER(4),"
        "[X] DOUBLE,"
        "[Y] DOUBLE,"
        "[WIDTH] DOUBLE,"
        "[HEIGHT] DOUBLE,"
        "[PYRLVL] INTEGER,"
        "[CUSTATTR1] INTEGER(4),"
        "[CUSTATTR2] INTEGER(4),"
        "[SUBBLK] INTEGER);";
    db->exec(ss.str());

    ss = stringstream();
    ss << "CREATE TABLE[" << CDbBase::TableName_TileData << "]("
        "[PIXELWIDTH] INTEGER(4),"
        "[PIXELHEIGHT] INTEGER(4),"
        "[PIXELTYPE] INTEGER(1),"
        "[DATATYPE] INTEGER(4),"
        "[DATA_BINHDR] BLOB(32),"
        "[DATA] BLOB);";
    db->exec(ss.str());

    ss = stringstream();
    ss << "CREATE VIRTUAL TABLE " << CDbBase::VTableName_SpatialTable << " USING rtree("
        "id,"           // Integer primary key
        "minX, maxX,"   // Minimum and maximum X coordinate"
        "minY, maxY"    // Minimum and maximum Y coordinate"
        ");";
    db->exec(ss.str());

    /*  static const char* CreateTileTableSqlStatement =
          R"(CREATE TABLE[TILETABLE](
              [PK] INTEGER NOT NULL PRIMARY KEY,
              [C] INTEGER(4),
              [T] INTEGER(4),
              [Z] INTEGER(4),
              [M] INTEGER(4),
              [X] DOUBLE,
              [Y] DOUBLE,
              [WIDTH] DOUBLE,
              [HEIGHT] DOUBLE,
              [PYRLVL] INTEGER,
              [CUSTATTR1] INTEGER(4),
              [CUSTATTR2] INTEGER(4),
              [SUBBLK] INTEGER);)";
      static const char* CreateSubBlkTableSqlStatement =
          R"(CREATE TABLE[SUBBLKTABLE](
              [PIXELWIDTH] INTEGER(4),
              [PIXELHEIGHT] INTEGER(4),
              [PIXELTYPE] INTEGER(1),
              [DATATYPE] INTEGER(4),
              [DATA_BINHDR] BLOB(32),
              [DATA] BLOB);)";
      static const char* CreateSpatialIndexSqlStatement =
          R"(CREATE VIRTUAL TABLE TILESPATIAL_index USING rtree(
               id,              -- Integer primary key
               minX, maxX,      -- Minimum and maximum X coordinate
               minY, maxY       -- Minimum and maximum Y coordinate
              );)";
      db->exec(CreateTileTableSqlStatement);
      db->exec(CreateSubBlkTableSqlStatement);
      db->exec(CreateSpatialIndexSqlStatement);*/
}
#endif

#if false
/*static*/IDbWrite* IDbFactory::CreateNew(const CreateOptions& opts)
{
    /* SQLite::Database* db = new SQLite::Database(opts.dbFilename, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
     try
     {
         CreateTileTable(db);
     }
     catch (SQLite::Exception & excp)
     {
         std::cout << excp.what();
     }*/
    auto docInfo = std::make_shared<CDbDocInfo>();
    docInfo->SetTileDimensions(opts.dimensions.cbegin(), opts.dimensions.cend());
    CDbCreation dbCreator(*docInfo, opts);
    auto db = dbCreator.DoCreate();

    return new CDbWrite(db, docInfo);
}
#endif
/*static*/std::shared_ptr<IDb> IDbFactory::CreateNew(const CreateOptions& opts)
{
    auto docInfo = std::make_shared<CDbDocInfo>();
    docInfo->SetTileDimensions(opts.dimensions.cbegin(), opts.dimensions.cend());
    map<IDbDocInfo::DbParameter, std::uint32_t> paramsMap;
    paramsMap[IDbDocInfo::DbParameter::DataBinHdrSize] = opts.sizeOfDataBinHdrField;
    docInfo->SetDbParameters(std::move(paramsMap));

    auto cc = DbDocInfoUtils::Convert(opts.perTileData);
    docInfo->SetCoordinateColumns(cc.cbegin(), cc.cend());

    CDbCreation dbCreator(*docInfo, opts);
    auto db = dbCreator.DoCreate();
    return make_shared<CDb>(db, docInfo);
}

/*static*/std::shared_ptr<IDb> IDbFactory::CreateNew3D(const CreateOptions& opts)
{
    auto docInfo = std::make_shared<CDbDocInfo3D>();
    docInfo->SetTileDimensions(opts.dimensions.cbegin(), opts.dimensions.cend());
    map<IDbDocInfo3D::DbParameter, std::uint32_t> paramsMap;
    paramsMap[IDbDocInfo3D::DbParameter::DataBinHdrSize] = opts.sizeOfDataBinHdrField;
    docInfo->SetDbParameters(std::move(paramsMap));

    auto cc = DbDocInfoUtils::Convert(opts.perTileData);
    docInfo->SetCoordinateColumns(cc.cbegin(), cc.cend());

    CDbCreation3D dbCreator(*docInfo, opts);
    auto db = dbCreator.DoCreate();

    return make_shared<CDb>(db, docInfo);
}

#if false
/*static*/IDbRead* IDbFactory::OpenExisting(const OpenOptions& opts)
{
    /* SQLite::Database* db = new SQLite::Database(opts.dbFilename, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
     try
     {
         CreateTileTable(db);
     }
     catch (SQLite::Exception & excp)
     {
         std::cout << excp.what();
     }*/
    SQLite::Database* db = new SQLite::Database(opts.dbFilename, SQLite::OPEN_READONLY);
    CCustomQueries::SetupCustomQueries(db->getHandle());
    CDbDiscover discover(db);
    auto docInfo = discover.GetDocInfo();

    return new CDbRead(db, docInfo);
}
#endif
/*static*/std::shared_ptr<IDb> IDbFactory::OpenExisting(const OpenOptions& opts)
{
    int aFlags = SQLite::OPEN_URI;
    if (opts.readOnly == true)
    {
        aFlags |= SQLite::OPEN_READONLY;
    }
    else
    {
        aFlags |= SQLite::OPEN_READWRITE;
    }

    SQLite::Database* db = new SQLite::Database(opts.dbFilename, aFlags);
    CCustomQueries::SetupCustomQueries(db->getHandle());

    CDbDiscover discover(db);
    discover.DoIt();

    auto docInfo = discover.GetDocInfo();
    if (docInfo)
    {
        return make_shared<CDb>(db, docInfo);
    }

    auto docInfo3d = discover.GetDocInfo3D();
    if (docInfo3d)
    {
        return make_shared<CDb>(db, docInfo3d);
    }

    return std::shared_ptr<IDb>();
}

//-------------------------------------------------------------------------------------------------

CDbCreation::CDbCreation(const IDbDocInfo& docInfo, const SlImgDoc::CreateOptions& opts)
    : docInfo(docInfo), opts(opts)
{}

SQLite::Database* CDbCreation::DoCreate()
{
    this->CheckCreateOptions(this->opts);

    try
    {
        SQLite::Database* db = new SQLite::Database(opts.dbFilename, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE | SQLite::OPEN_URI);
        CCustomQueries::SetupCustomQueries(db->getHandle());

        CDbMasterInfoTableHelper::AddMasterTable(db, "0.0.1-alpha");
        CDbMasterInfoTableHelper::AddDocumentTiles2D(
            db,
            CDbDocInfo::GetDefaultTableName);

        auto sqlStatement = this->GetTilesInfoCreateSqlStatement();
        db->exec(sqlStatement);
        sqlStatement = this->GetTilesDataCreateSqlStatement();
        db->exec(sqlStatement);

        if (this->opts.createSpatialIndex)
        {
            sqlStatement = this->GetTilesSpatialIndexCreateSqlStatement();
            db->exec(sqlStatement);
        }

        sqlStatement = this->GetPerTileDataTableSqlStatement();
        if (!sqlStatement.empty())
        {
            db->exec(sqlStatement);
        }

        return db;
    }
    catch (SQLite::Exception & excp)
    {
        std::cout << excp.what();
        throw;
    }
}

std::string CDbCreation::GetTilesInfoCreateSqlStatement() const
{
    auto ss = stringstream();
    ss << "CREATE TABLE[" << this->docInfo.GetTableName(IDbDocInfo::TableType::TilesInfo) << "]("
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::Pk) << "] INTEGER NOT NULL PRIMARY KEY,"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileX) << "] DOUBLE NOT NULL,"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileY) << "] DOUBLE NOT NULL,"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileWidth) << "] DOUBLE NOT NULL,"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileHeight) << "] DOUBLE NOT NULL,"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::PyrLvl) << "] INTEGER(1),"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileDataId) << "] INTEGER(8)";

    const auto tileDims = this->docInfo.GetTileDimensions();
    for (const auto dim : tileDims)
    {
        string colName;
        bool b = this->docInfo.GetTileInfoColumnNameForDimension(dim, colName);
        ss << ", [" << colName << "] INTEGER(4)";
    }

    ss << ");";
    return ss.str();
}

std::string CDbCreation::GetTilesDataCreateSqlStatement() const
{
    auto ss = stringstream();
    ss << "CREATE TABLE[" << this->docInfo.GetTableName(IDbDocInfo::TableType::TilesData) << "]("
        "[" << this->docInfo.GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelWidth) << "] INTEGER(4),"
        "[" << this->docInfo.GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelHeight) << "] INTEGER(4),"
        "[" << this->docInfo.GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelType) << "] INTEGER(1),"
        "[" << this->docInfo.GetTileDataColumnName(IDbDocInfo::TilesDataColumn::DataType) << "] INTEGER(4),"
        "[" << this->docInfo.GetTileDataColumnName(IDbDocInfo::TilesDataColumn::DataBinHdr) << "] BLOB(" << this->opts.sizeOfDataBinHdrField << "),"
        "[" << this->docInfo.GetTileDataColumnName(IDbDocInfo::TilesDataColumn::Data) << "] BLOB);";
    return ss.str();
}

std::string CDbCreation::GetTilesSpatialIndexCreateSqlStatement() const
{
    auto ss = stringstream();
    ss << "CREATE VIRTUAL TABLE " << this->docInfo.GetTableName(IDbDocInfo::TableType::TilesSpatialIndex) << " USING rtree(" <<
        this->docInfo.GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::Pk) << "," <<         // Integer primary key
        this->docInfo.GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinX) << "," <<       // Minimum X coordinate"
        this->docInfo.GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxX) << "," <<       // Maximum X coordinate"
        this->docInfo.GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinY) << "," <<       // Minimum Y coordinate"
        this->docInfo.GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxY) << ");";        // Maximum Y coordinate"
    return ss.str();
}

std::string CDbCreation::GetPerTileDataTableSqlStatement() const
{
    struct Adapter
    {
        const CDbCreation& r;
        Adapter(const CDbCreation& r) :r(r) {};

        const std::vector<ColumnTypeAllInfo>& GetPerTileColumnInfo() const { return this->r.docInfo.GetCoordinateDataColumnInfo(); }
        std::string GetTableNamePerTileData() const { return this->r.docInfo.GetTableName(IDbDocInfo::TableType::PerBrickData); }
        std::string GetColumnNamePk() const { return this->r.docInfo.GetPerTilesDataColumnName(IDbDocInfo::PerTileDataColumn::Pk); }
        std::string GetTableNameTilesInfo() const { return this->r.docInfo.GetTableName(IDbDocInfo::TableType::TilesInfo); }
        std::string GetColumnNamePkTilesInfo() const { return this->r.docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::Pk); }
    };

    return this->GetPerTileDataTableSqlStatementCommon(Adapter(*this));
#if false
    const auto& coordinateDataColInfo = this->docInfo.GetCoordinateDataColumnInfo();
    if (coordinateDataColInfo.empty())
    {
        return string();
    }

    auto ss = stringstream();
    ss << "CREATE TABLE[" << this->docInfo.GetTableName(IDbDocInfo::TableType::PerBrickData) << "](";

    /* const auto tileDims = this->docInfo.GetTileDimensions();
     bool isFirst = true;
     for (const auto dim : tileDims)
     {
         string colName;
         bool b = this->docInfo.GetCoordinateDataColumnNameForDimension(dim, colName);
         if (isFirst == false)
         {
             ss << ",";

         }
         ss << "[" << colName << "] INTEGER(4)";
         isFirst = false;
     }*/
    ss << this->docInfo.GetPerTilesDataColumnName(IDbDocInfo::PerTileDataColumn::Pk) << " INTEGER";

    for (const auto& ci : this->docInfo.GetCoordinateDataColumnInfo())
    {
        switch (ci.type)
        {
        case ColumnType::Integer:
            ss << ",[" << ci.columnName << "] INTEGER(" << ci.size << ")";
            break;
        case ColumnType::Float:
            ss << ",[" << ci.columnName << "] DOUBLE";
            break;
        }
    }

    ss << ",FOREIGN KEY(" << this->docInfo.GetPerTilesDataColumnName(IDbDocInfo::PerTileDataColumn::Pk) << ") REFERENCES " <<
        this->docInfo.GetTableName(IDbDocInfo::TableType::TilesInfo) << "(" << this->docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::Pk) << ")";

    ss << ");";

    return ss.str();
#endif
}

/// Checks the create-options for validity. In the case that the data is determined to be invalid,
/// an exception of type SlImgDoc::SqliteImgDocException thrown.
/// \exception SlImgDoc::SqliteImgDocException Thrown when the data is determined to be invalid.
/// \param opts The create-options to be validated.
void CDbCreation::CheckCreateOptions(const SlImgDoc::CreateOptions& opts)
{
    if (opts.sizeOfDataBinHdrField <= 0)
    {
        stringstream ss;
        ss << "The value \"" << opts.sizeOfDataBinHdrField << "\" for the field 'sizeOfDataBinHdrField' is invalid.";
        throw SqliteImgDocInvalidArgumentException(ss.str());
    }
}
//
//void CDbCreation::CreateCoordinateDataTable()
//{
//    auto ss = stringstream();
//    ss << "CREATE TABLE[" << this->docInfo.GetTableName(IDbDocInfo::TableType::CoordinateData) <<"](";
//
//    const auto tileDims = this->docInfo.GetTileDimensions();
//    bool isFirst = true;
//    for (const auto dim : tileDims)
//    {
//        string colName;
//        bool b = this->docInfo.GetCoordinateDataColumnNameForDimension(dim, colName);
//        if (isFirst==false)
//        {
//            ss << ",";
//
//        }
//        ss << "[" << colName << "] INTEGER(4),";
//        isFirst = false;
//    }
//
//    for (const auto& ci:this->docInfo.GetCoordinateDataColumnInfo())
//    {
//        switch (ci.type)
//        {
//        case ColumnType::Integer:
//            ss << ",[" << ci.columnName<<"] INTEGER(" << ci.size << ")";
//        case ColumnType::Float:
//            ss << ",[" << ci.columnName << "] DOUBLE";
//        }
//    }
//
//    ss << ");";
//}