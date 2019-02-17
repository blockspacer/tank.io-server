
 set(MATH_SHAPE_SOURCE)
 set(MATH_SHAPE_HEADER)



 file(GLOB MATH_SHAPE_SOURCE
      "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/*/*.cpp"
 )
 file(GLOB MATH_SHAPE_HEADER
      "${CMAKE_CURRENT_LIST_DIR}/*.h"
      "${CMAKE_CURRENT_LIST_DIR}/*/*.h"
 )
include_directories(
    ${CMAKE_CURRENT_LIST_DIR}
    )
