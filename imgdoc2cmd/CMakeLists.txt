add_executable(imgdoc2cmd 
                imgdoc2cmd.cpp)

target_link_libraries(imgdoc2cmd  libimgdoc2_a)

#target_include_directories(imgdoc2cmd PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../extlibs/cxxopts/>)
