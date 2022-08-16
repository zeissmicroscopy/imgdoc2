add_library(SQLiteCpp STATIC 
src/Backup.cpp
src/Column.cpp
src/Database.cpp
src/Exception.cpp
src/Savepoint.cpp
src/Statement.cpp
src/Transaction.cpp

include/SQLiteCpp/Assertion.h
include/SQLiteCpp/Backup.h
include/SQLiteCpp/Column.h
include/SQLiteCpp/Database.h
include/SQLiteCpp/Exception.h
include/SQLiteCpp/ExecuteMany.h
include/SQLiteCpp/Savepoint.h
include/SQLiteCpp/SQLiteCpp.h
include/SQLiteCpp/Statement.h
include/SQLiteCpp/Transaction.h
include/SQLiteCpp/Utils.h
include/SQLiteCpp/VariadicBind.h
)

#set_target_properties(SQLiteCpp PROPERTIES INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../sqlite3>)
#set_target_properties(SQLiteCpp PROPERTIES INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../external/sqlite3>)
target_link_libraries(SQLiteCpp PUBLIC  sqlite3)
target_include_directories(SQLiteCpp PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_include_directories(SQLiteCpp PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../sqlite3>)

target_include_directories(SQLiteCpp INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../sqlite3>)

#[[
set_target_properties(SQLiteCpp PROPERTIES
    OUTPUT_NAME   SQLiteCpp
    IMPORTED_LOCATION  "${CMAKE_SOURCE_DIR}/include"
    DEBUG_POSTFIX d
    )
]]

    #[[
# Configuration
# All options are defined at:
# https://www.sqlite.org/compile.html
target_compile_definitions(SQLiteCpp PUBLIC # inject user's options
  SQLITE_ENABLE_RTREE 
  SQLITE_ENABLE_COLUMN_METADATA
  SQLITE_USE_ALLOCA
  SQLITE_LIKE_DOESNT_MATCH_BLOBS
)

]]