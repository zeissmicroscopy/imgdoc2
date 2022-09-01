add_executable(imgdoc2cmd 
                imgdoc2cmd.cpp)

#target_include_directories(imgdoc2cmd PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../extlibs/cxxopts/>)

if(UNIX AND NOT APPLE)
   # Linux
   message(STATUS "****** UNIX AND NOT APPLE ******")
   target_link_libraries(imgdoc2cmd PRIVATE libimgdoc2_v2 ${CMAKE_THREAD_LIBS_INIT}  ${CMAKE_DL_LIBS})
elseif(UNIX AND APPLE)
   # Mac OS
   target_link_libraries(imgdoc2cmd PRIVATE libimgdoc2_v2 pthread dl)
else()
   target_link_libraries(imgdoc2cmd PRIVATE  libimgdoc2_v2 )
endif()
