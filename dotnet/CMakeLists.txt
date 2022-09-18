
message(STATUS "###### HI ######")

#[[
add_custom_target(
  create_txt
  DEPENDS imgdoc2API
  COMMAND ${CMAKE_COMMAND} -E echo  "*** HELLO WORLD ***" "$<TARGET_FILE_BASE_NAME:imgdoc2API>")

]]
