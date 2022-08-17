set(LibImgDoc2_aSrcfiles 

src/public/ClassFactory.cpp
src/public/CreateOptions.cpp

inc/CreateOptions.h
         "inc/ClassFactory.h" "inc/imgdoc2.h" "src/db/database_configuration.h" "src/db/database_configuration.cpp")

add_library(libimgdoc2_a STATIC
                ${LibImgDoc2_aSrcfiles}
)

target_link_libraries(libimgdoc2_a PUBLIC SQLiteCpp sqlite3)
set_target_properties(libimgdoc2_a PROPERTIES INCLUDE_DIRECTORIES "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../external/sqlite3>;${CMAKE_CURRENT_SOURCE_DIR}/inc")

# this preprocessor define needs to be defined when building "SqliteImgDoc" and by users of it (if linking the static library)
target_compile_definitions(libimgdoc2_a PUBLIC _SQLITEIMGDOCSTATICLIB=1)