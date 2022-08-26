set(LibImgDoc2_aSrcfiles 

src/public/ClassFactory.cpp
src/public/CreateOptions.cpp

inc/CreateOptions.h
         "inc/ClassFactory.h" 
         "inc/imgdoc2.h" 
         "src/db/database_configuration.h" 
         "src/db/database_configuration.cpp" 
         "src/db/database_creator.h" 
         "src/db/database_creator.cpp" 
         "src/db/IDbConnection.h" 
         "src/db/IDb.h" 
         "inc/IDocRead2d.h" 
         "inc/IDocWrite2d.h" 
         "src/db/DbFactory.h" 
         "src/db/sqlite/sqlite_DbFactory.cpp" 
         "src/db/sqlite/sqlite_DbConnection.cpp" 
         "src/db/sqlite/sqlite_DbConnection.h" 
         "inc/IDoc.h" 
         "src/doc/document.h" 
         "src/doc/document.cpp" 
         "src/db/sqlite/custom_functions.cpp"
         "src/db/sqlite/custom_functions.h" 
         "inc/types.h"  
         "inc/IDbWriteTransaction.h" 
         "inc/IDbIndexManagement.h" 
         "src/doc/documentRead2d.h" 
         "src/doc/documentRead2d.cpp" 
         "inc/LogicalPositionInfo.h" 
         "inc/utilities.h" 
         "inc/TileCoordinate.h" 
         "inc/ITileCoordinate.h" 
         "src/db/sqlite/sqlite_DbStatement.h" 
         "src/db/sqlite/sqlite_DbStatement.cpp" 
         "src/db/sqlite/ISqlite_DbStatement.h" 
         "src/doc/documentWrite2d.h"
         "src/doc/documentWrite2d.cpp"
         "inc/DataTypes.h" 
         "inc/IDataObj.h" 
         "src/db/utilities.h" 
         "src/db/utilities.cpp")

add_library(libimgdoc2_a STATIC
                ${LibImgDoc2_aSrcfiles}
)

target_link_libraries(libimgdoc2_a PUBLIC SQLiteCpp sqlite3)
set_target_properties(libimgdoc2_a PROPERTIES INCLUDE_DIRECTORIES "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../external/sqlite3>;${CMAKE_CURRENT_SOURCE_DIR}/inc")

# this preprocessor define needs to be defined when building "SqliteImgDoc" and by users of it (if linking the static library)
target_compile_definitions(libimgdoc2_a PUBLIC _SQLITEIMGDOCSTATICLIB=1)

target_include_directories(libimgdoc2_a INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc> 
  )