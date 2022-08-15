#include "pch.h"

using namespace std;
using namespace SlImgDoc;

TEST(DbCreateDbTests, ParameterValidationTest_InvalidSizeOfDataBinHdrWhenCreatingDb)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.sizeOfDataBinHdrField = -1;    // this must be greater than zero, so we expect this to fail
    EXPECT_THROW(IDbFactory::CreateNew(opts), SlImgDoc::SqliteImgDocInvalidArgumentException) << "An exception was expected here.";

    opts.sizeOfDataBinHdrField = 0;
    EXPECT_THROW(IDbFactory::CreateNew(opts), SlImgDoc::SqliteImgDocInvalidArgumentException) << "An exception was expected here.";
}

TEST(DbCreateDbTests, InvalidSizeOfDataBinHdr1)
{
    // we create the database with a databin-header of size 18
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.sizeOfDataBinHdrField = 18;
    auto db = IDbFactory::CreateNew(opts);

    auto dbWrite = db->GetWriter();

    LogicalPositionInfo posInfo;
    posInfo.width = 100;
    posInfo.height = 100;
    posInfo.pyrLvl = 0;
    TileBaseInfo tileBaseInfo;
    tileBaseInfo.pixelWidth = 100;
    tileBaseInfo.pixelHeight = 100;
    tileBaseInfo.pixelType = PixelType::GRAY8;

    TileCoordinate tc({ { 'C',0 },{ 'Z',0 } });

    // ...and then try to use a 19 bytes databin-header-object, which is expect to fail
    CDataObjCustom dataCustom(19, 1);

    EXPECT_THROW(dbWrite->AddTile(&tc, &posInfo, &tileBaseInfo, DataTypes::CUSTOM, &dataCustom), SlImgDoc::SqliteImgDocInvalidArgumentException) << "An exception was expected here.";
}

TEST(DbCreateDbTests, InvalidSizeOfDataBinHdr2)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.sizeOfDataBinHdrField = 18;
    auto db = IDbFactory::CreateNew3D(opts);

    auto dbWrite = db->GetWriter3D();

    LogicalPositionInfo3D posInfo;
    posInfo.width = 100;
    posInfo.height = 100;
    posInfo.depth = 100;
    posInfo.pyrLvl = 0;
    TileBaseInfo3D tileBaseInfo;
    tileBaseInfo.pixelWidth = 100;
    tileBaseInfo.pixelHeight = 100;
    tileBaseInfo.pixelDepth = 100;
    tileBaseInfo.pixelType = PixelType::GRAY8;

    TileCoordinate tc({ { 'C',0 },{ 'Z',0 } });
    CDataObjCustom dataCustom(19, 1);

    EXPECT_THROW(dbWrite->AddBrick(&tc, &posInfo, &tileBaseInfo, DataTypes::CUSTOM ,&dataCustom), SlImgDoc::SqliteImgDocInvalidArgumentException) << "An exception was expected here.";
}

TEST(DbCreateDbTests, InsufficientCoordinate1)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.sizeOfDataBinHdrField = 4;
    auto db = IDbFactory::CreateNew(opts);

    auto dbWrite = db->GetWriter();

    LogicalPositionInfo posInfo;
    posInfo.width = 100;
    posInfo.height = 100;
    posInfo.pyrLvl = 0;
    TileBaseInfo tileBaseInfo;
    tileBaseInfo.pixelWidth = 100;
    tileBaseInfo.pixelHeight = 100;
    tileBaseInfo.pixelType = PixelType::GRAY8;

    TileCoordinate tc({ { 'C',0 } });
    CDataObjCustom dataCustom(19, 1);

    EXPECT_THROW(dbWrite->AddTile(&tc, &posInfo, &tileBaseInfo, DataTypes::CUSTOM, &dataCustom), SlImgDoc::SqliteImgDocInvalidArgumentException) << "An exception was expected here.";
}

TEST(DbCreateDbTests, InsufficientCoordinate2)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');       // here we declase that a tile-coordinate is given by
    opts.dimensions.emplace('Z');       // dimension 'C' and 'Z'
    opts.sizeOfDataBinHdrField = 4;
    auto db = IDbFactory::CreateNew(opts);

    auto dbWrite = db->GetWriter();

    LogicalPositionInfo posInfo;
    posInfo.width = 100;
    posInfo.height = 100;
    posInfo.pyrLvl = 0;
    TileBaseInfo tileBaseInfo;
    tileBaseInfo.pixelWidth = 100;
    tileBaseInfo.pixelHeight = 100;
    tileBaseInfo.pixelType = PixelType::GRAY8;

    // and now we add a coordinate which has dimension 'C' and 'X'
    TileCoordinate tc({ { 'C',0 },{'X',2} });
    CDataObjCustom dataCustom(19, 1);

    // so we expect an exception
    EXPECT_THROW(dbWrite->AddTile(&tc, &posInfo, &tileBaseInfo, DataTypes::CUSTOM, &dataCustom), SlImgDoc::SqliteImgDocInvalidArgumentException) << "An exception was expected here.";
}

TEST(DbCreateDbTests, CreateAndDiscover2D)
{
    CreateOptions opts;

    // Note: the syntax below allows to have multiple connections to the "in-memory-data database",
    //        cf. https://sqlite.org/inmemorydb.html
    opts.dbFilename = "file:memdb1?mode=memory&cache=shared";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.sizeOfDataBinHdrField = 4;
    auto db = IDbFactory::CreateNew(opts);
    auto dbWrite = db->GetWriter();

    LogicalPositionInfo posInfo;
    posInfo.width = 100;
    posInfo.height = 100;
    posInfo.pyrLvl = 0;
    TileBaseInfo tileBaseInfo;
    tileBaseInfo.pixelWidth = 100;
    tileBaseInfo.pixelHeight = 100;
    tileBaseInfo.pixelType = PixelType::GRAY8;
    TileCoordinate tc({ { 'C',0 },{'Z',2} });
    CDataObjCustom dataCustom(4, 1);
    dbWrite->AddTile(&tc, &posInfo, &tileBaseInfo, DataTypes::CUSTOM, &dataCustom);

    OpenOptions openOpts;
    openOpts.dbFilename = "file:memdb1?mode=memory&cache=shared";
    auto db2 = IDbFactory::OpenExisting(openOpts);
    auto dbRead = db2->GetReader();
    auto dbRead3d = db2->GetReader3D();
    EXPECT_TRUE(dbRead) << "Expecting to have a valid data-base here.";
    EXPECT_FALSE(dbRead3d) << "Expecting NOT to have a valid data-base here.";
}

TEST(DbCreateDbTests, CreateAndDiscover3D)
{
    CreateOptions opts;
    opts.dbFilename = "file:memdb2?mode=memory&cache=shared";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.sizeOfDataBinHdrField = 4;
    auto db = IDbFactory::CreateNew3D(opts);
    auto dbWrite = db->GetWriter3D();

    LogicalPositionInfo3D posInfo;
    posInfo.width = 100;
    posInfo.height = 100;
    posInfo.depth = 100;
    posInfo.pyrLvl = 0;
    TileBaseInfo3D tileBaseInfo;
    tileBaseInfo.pixelWidth = 100;
    tileBaseInfo.pixelHeight = 100;
    tileBaseInfo.pixelDepth = 100;
    tileBaseInfo.pixelType = PixelType::GRAY8;
    TileCoordinate tc({ { 'C',0 },{'Z',2} });
    CDataObjCustom dataCustom(4, 1);
    dbWrite->AddBrick(&tc, &posInfo, &tileBaseInfo, DataTypes::CUSTOM ,&dataCustom);

    OpenOptions openOpts;
    openOpts.dbFilename = "file:memdb2?mode=memory&cache=shared";
    auto db2 = IDbFactory::OpenExisting(openOpts);
    auto dbRead = db2->GetReader();
    auto dbRead3d = db2->GetReader3D();
    EXPECT_TRUE(dbRead3d) << "Expecting to have a valid data-base here.";
    EXPECT_FALSE(dbRead) << "Expecting NOT to have a valid data-base here.";
}

TEST(DbCreateDbTests, CreateAndDiscover2D_2)
{
    CreateOptions opts;
    opts.dbFilename = "file:memdb5?mode=memory&cache=shared";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.sizeOfDataBinHdrField = 4;
    opts.perTileData.descriptions.push_back(ColumnDescription{ "A","FLOAT" });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "B","INTEGER(1)" });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "C","INTEGER(2)" });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "D","INTEGER(4)" });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "E","INTEGER(8)" });
    auto db = IDbFactory::CreateNew(opts);
    auto dbWrite = db->GetWriter();

    OpenOptions openOpts;
    openOpts.dbFilename = opts.dbFilename;
    auto db2 = IDbFactory::OpenExisting(openOpts);
    auto dbRead = db2->GetReader();
    auto dbRead3d = db2->GetReader3D();
    EXPECT_TRUE(dbRead) << "Expecting to have a valid data-base here.";
    EXPECT_FALSE(dbRead3d) << "Expecting NOT to have a valid data-base here.";

    auto perTileColumns = dbRead->GetPerTileColumns();

    for (const auto& i : opts.perTileData.descriptions)
    {
        const auto& it = find_if(
            perTileColumns.cbegin(),
            perTileColumns.cend(),
            [&](const ColumnDescription& cd)->bool {return cd.Name == i.Name; });
        ASSERT_TRUE(it != perTileColumns.cend());
        ASSERT_TRUE(it->DataType == i.DataType);
    }
}

TEST(DbCreateDbTests, CreateAndDiscover3D_2)
{
    CreateOptions opts;
    opts.dbFilename = "file:memdb6?mode=memory&cache=shared";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.sizeOfDataBinHdrField = 4;
    opts.perTileData.descriptions.push_back(ColumnDescription{ "A","FLOAT" });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "B","INTEGER(1)" });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "C","INTEGER(2)" });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "D","INTEGER(4)" });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "E","INTEGER(8)" });
    auto db = IDbFactory::CreateNew3D(opts);
    auto dbWrite = db->GetWriter3D();

    LogicalPositionInfo3D posInfo;
    posInfo.width = 100;
    posInfo.height = 100;
    posInfo.depth = 100;
    posInfo.pyrLvl = 0;
    TileBaseInfo3D tileBaseInfo;
    tileBaseInfo.pixelWidth = 100;
    tileBaseInfo.pixelHeight = 100;
    tileBaseInfo.pixelDepth = 100;
    tileBaseInfo.pixelType = PixelType::GRAY8;
    TileCoordinate tc({ { 'C',0 },{'Z',2} });
    CDataObjCustom dataCustom(4, 1);
    dbWrite->AddBrick(&tc, &posInfo, &tileBaseInfo, DataTypes::CUSTOM, &dataCustom);

    OpenOptions openOpts;
    openOpts.dbFilename = opts.dbFilename;
    auto db2 = IDbFactory::OpenExisting(openOpts);
    auto dbRead = db2->GetReader();
    auto dbRead3d = db2->GetReader3D();
    EXPECT_TRUE(dbRead3d) << "Expecting to have a valid data-base here.";
    EXPECT_FALSE(dbRead) << "Expecting NOT to have a valid data-base here.";

    auto perTileColumns = dbRead3d->GetPerTileColumns();

    for (const auto& i : opts.perTileData.descriptions)
    {
        const auto& it = find_if(
            perTileColumns.cbegin(),
            perTileColumns.cend(),
            [&](const ColumnDescription& cd)->bool {return cd.Name == i.Name; });
        ASSERT_TRUE(it != perTileColumns.cend());
        ASSERT_TRUE(it->DataType == i.DataType);
    }
}

TEST(DbCreateDbTests, CreateWithSpatialIndexAndCheckWhetherSpatialIndexExists)
{
    CreateOptions opts;
    // Note: the syntax below allows to have multiple connections to the "in-memory-data database",
    //        cf. https://sqlite.org/inmemorydb.html
    opts.dbFilename = "file:memdb7?mode=memory&cache=shared";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.sizeOfDataBinHdrField = 4;
    auto db = IDbFactory::CreateNew(opts);
    auto dbWrite = db->GetWriter();

    LogicalPositionInfo posInfo;
    posInfo.width = 100;
    posInfo.height = 100;
    posInfo.pyrLvl = 0;
    TileBaseInfo tileBaseInfo;
    tileBaseInfo.pixelWidth = 100;
    tileBaseInfo.pixelHeight = 100;
    tileBaseInfo.pixelType = PixelType::GRAY8;
    TileCoordinate tc({ { 'C',0 },{'Z',2} });
    CDataObjCustom dataCustom(4, 1);
    dbWrite->AddTile(&tc, &posInfo, &tileBaseInfo, DataTypes::CUSTOM, &dataCustom);

    OpenOptions openOpts;
    openOpts.dbFilename = "file:memdb7?mode=memory&cache=shared";
    auto db2 = IDbFactory::OpenExisting(openOpts);
    auto dbRead = db2->GetReader();
    bool isIndexed = dbRead->IsTilePositionExtentIndexed();
    ASSERT_TRUE(isIndexed) << "Expected to have a spatial index";
}

TEST(DbCreateDbTests, CreateWithoutSpatialIndexAndCheckWhetherSpatialIndexDoesNotExist)
{
    CreateOptions opts;
    opts.dbFilename = "file:memdb8?mode=memory&cache=shared";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.sizeOfDataBinHdrField = 4;
    opts.createSpatialIndex = false;
    auto db = IDbFactory::CreateNew(opts);
    auto dbWrite = db->GetWriter();

    LogicalPositionInfo posInfo;
    posInfo.width = 100;
    posInfo.height = 100;
    posInfo.pyrLvl = 0;
    TileBaseInfo tileBaseInfo;
    tileBaseInfo.pixelWidth = 100;
    tileBaseInfo.pixelHeight = 100;
    tileBaseInfo.pixelType = PixelType::GRAY8;
    TileCoordinate tc({ { 'C',0 },{'Z',2} });
    CDataObjCustom dataCustom(4, 1);
    dbWrite->AddTile(&tc, &posInfo, &tileBaseInfo, DataTypes::CUSTOM, &dataCustom);

    OpenOptions openOpts;
    openOpts.dbFilename = "file:memdb8?mode=memory&cache=shared";
    auto db2 = IDbFactory::OpenExisting(openOpts);
    auto dbRead = db2->GetReader();
    bool isIndexed = dbRead->IsTilePositionExtentIndexed();
    ASSERT_FALSE(isIndexed) << "Expected to not find a spatial index";
}
