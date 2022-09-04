# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.7)

include(FetchContent)

project("imgdoc2")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(IMGDOC2_BUILD_CLANGTIDY "Build with Clang-Tidy" OFF)

if (IMGDOC2_BUILD_CLANGTIDY)
  set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*,-llvm-*,-llvmlibc-*,fuchsia-*,-google-build-using-namespace,-modernize-use-trailing-return-type")
  #set(CMAKE_C_CLANG_TIDY "clang-tidy;-checks=*")
endif()

include(ExternalProject)

include(FetchContent)

# In this example we are picking a specific tag.
# You can also pick a specific commit, if you need to.
FetchContent_Declare(GSL
    GIT_REPOSITORY "https://github.com/microsoft/GSL"
    GIT_TAG "v4.0.0"
)

FetchContent_MakeAvailable(GSL)


# Include sub-projects.
#find_package(sqlite3 CONFIG REQUIRED)
#find_package(SQLiteCpp REQUIRED)

#[[
ExternalProject_Add(sqlite3
  URL               https://www.sqlite.org/2022/sqlite-amalgamation-3390200.zip
  URL_HASH          SHA3_256=deb2abef617b6305525e3b1a2b39a5dc095ffb62f243b4d1b468ba5f41900ce7
  CONFIGURE_COMMAND ""
  PATCH_COMMAND     ${CMAKE_COMMAND} -E copy "${imgdoc2_SOURCE_DIR}/cmake/sqlite_cmakelists.txt" "${CMAKE_CURRENT_BINARY_DIR}/sqlite3-prefix/src/sqlite3/CMakeLists.txt"

  #--Configure step-------------
  SOURCE_DIR        ${CMAKE_SOURCE_DIR}/source
  CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_SOURCE_DIR}/install
                      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                      -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
  #--Build step-----------------
  BINARY_DIR        ${CMAKE_SOURCE_DIR}/build
  BUILD_COMMAND     ${CMAKE_COMMAND} --build .
  #--Install step---------------
  INSTALL_DIR       ${CMAKE_SOURCE_DIR}/install
)

add_subdirectory("${CMAKE_CURRENT_BINARY_DIR}/sqlite3-prefix/src/sqlite3")
]]

add_subdirectory(external/sqlite3)
#add_subdirectory(external/SQLiteCpp)

#[[
FetchContent_Declare(
  SQLiteCpp
  GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
	GIT_TAG "3.1.1"
)

if(NOT SQLiteCpp_POPULATED)
	message(STATUS "Fetching SQLiteCpp...")

  get_target_property(SQLITE3BINARYDIR sqlite3 BINARY_DIR)
  set(SQLite3_LIBRARY    ${SQLITE3BINARYDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}sqlite3${CMAKE_STATIC_LIBRARY_SUFFIX} )
  #set(SQLITECPP_INTERNAL_SQLITE OFF CACHE BOOL "" FORCE)
  message(STATUS "******** SQLite3_LIBRARY=${SQLite3_LIBRARY} ********")
  set(SQLite3_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/external/sqlite3)
  message(STATUS "******** ${SQLite3_INCLUDE_DIR} ********")


#  get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

  #set(SQLite3_INCLUDE_DIR 
	set(SQLITECPP_RUN_CPPCHECK OFF CACHE BOOL "" FORCE)
  set(SQLITECPP_RUN_CPPLINT OFF CACHE BOOL "" FORCE)
  set(SQLITECPP_INTERNAL_SQLITE OFF CACHE BOOL "" FORCE) # here we instruct it to NOT use the "bundled" SQLite-version
  set(SQLITECPP_INCLUDE_SCRIPT OFF CACHE BOOL "" FORCE)
  set(SQLITECPP_RUN_CPPLINT OFF CACHE BOOL "" FORCE)
  set(SQLITECPP_RUN_CPPCHECK OFF CACHE BOOL "" FORCE)
  set(SQLITECPP_USE_STATIC_RUNTIME OFF CACHE BOOL "" FORCE)
  set(SQLITE_ENABLE_RTREE ON CACHE BOOL "" FORCE)


	FetchContent_Populate(SQLiteCpp)

  message(STATUS "SQLiteCpp_SOURCE_DIR=${sqlitecpp_SOURCE_DIR} SQLiteCpp_BINARY_DIR=${sqlitecpp_BINARY_DIR}")
	add_subdirectory(${sqlitecpp_SOURCE_DIR} ${sqlitecpp_BINARY_DIR})
endif()
]]

#add_subdirectory(libimgdoc2)
add_subdirectory(libimgdoc2_v2)

add_subdirectory(imgdoc2cmd)

enable_testing()

add_subdirectory(libimgdoc2_v2_tests)
#add_subdirectory(libimgdoc2_tests)



