set(LibImgDoc2Srcfiles 

src/BlobOnHeap.cpp
src/CCustomQueries.cpp
src/CCustomQueries.h
src/CoordinateData.cpp
src/Db.cpp
src/Db.h
src/DbBase.cpp
src/DbBase.h
src/DbCreation.cpp
src/DbCreation.h
src/DbCreation3D.cpp
src/DbCreation3D.h
src/DbCreationCommon.h
src/DbDiscover.cpp
src/DbDiscover.h
src/DbGlobalInfo.cpp
src/DbGlobalInfo.h
src/DbGlobalInfo3D.cpp
src/DbGlobalInfo3D.h
src/DbMasterInfoTable.cpp
src/DbMasterInfoTable.h
src/DbRead.cpp
src/DbRead.h
src/DbRead3D.cpp
src/DbRead3D.h
src/DbReadCommon.h
src/dbutils.cpp
src/dbutils.h
src/DbWrite.cpp
src/DbWrite.h
src/DbWrite3D.cpp
src/DbWrite3D.h
src/DbWriteBase.cpp
src/DbWriteBase.h
src/IInternalDb.h
src/miscutilities.cpp
src/miscutilities.h
src/pch.cpp
src/pch.h
src/QueryBuildUtils.cpp
src/QueryBuildUtils.h

inc/BlobData.h
inc/BlobOnHeap.h
inc/CoordinateData.h
inc/DataBinHdr.h
inc/DataObjImpl.h
inc/DataTypes.h
inc/DimCoordinateQueryClause.h
inc/IBlob.h
inc/IDataObj.h
inc/IDimCoordinateQueryClause.h
inc/ImportExport.h
inc/Interface.h
inc/ITileCoordinate.h
inc/ITileInfoQueryClause.h
inc/LogicalPositionInfo.h
inc/SqliteImgDocException.h
inc/TileCoordinate.h
inc/TileInfoQueryClause.h
inc/Types.h
inc/utilities.h
        )

add_library(libimgdoc2 STATIC
                ${LibImgDoc2Srcfiles}
)

target_link_libraries(libimgdoc2 PUBLIC SQLiteCpp sqlite3)
set_target_properties(libimgdoc2 PROPERTIES INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../external/sqlite3>)

# this preprocessor define needs to be defined when building "SqliteImgDoc" and by users of it (if linking the static library)
target_compile_definitions(libimgdoc2 PUBLIC _SQLITEIMGDOCSTATICLIB=1)