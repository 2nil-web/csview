include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_BINARY_DIR}")
include(GenerateExportHeader)
add_library(edit_a STATIC editline.c fn_complete.c history.c)
add_library(edit SHARED editline.c fn_complete.c history.c)
set_target_properties(edit PROPERTIES PREFIX "")
GENERATE_EXPORT_HEADER(edit
  BASE_NAME edit
  EXPORT_MACRO_NAME edit_EXPORT
  EXPORT_FILE_NAME edit_Export.h
  STATIC_DEFINE edit_BUILT_AS_STATIC
)
install (TARGETS edit_a
  DESTINATION "${CMAKE_SOURCE_DIR}/lib${LIB_SUFFIX}")
install (FILES editline/readline.h
  DESTINATION "${CMAKE_SOURCE_DIR}/include/editline")
string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)
if (MSVC AND uppercase_CMAKE_BUILD_TYPE MATCHES "DEBUG")
  install (FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/edit_a.pdb
    DESTINATION "${CMAKE_SOURCE_DIR}/lib${LIB_SUFFIX}")
  install (FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/edit.pdb
    DESTINATION "${CMAKE_SOURCE_DIR}/bin${LIB_SUFFIX}")
endif()
