
 set(GAME_CORE_SOURCE)
 set(GAME_CORE_HEADER)



 file(GLOB GAME_CORE_SOURCE
      "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/*/*.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/*/*/*.cpp"
 )
 file(GLOB GAME_CORE_HEADER
      "${CMAKE_CURRENT_LIST_DIR}/*.h"
      "${CMAKE_CURRENT_LIST_DIR}/*/*.h"
      "${CMAKE_CURRENT_LIST_DIR}/*/*/*.h"
 )
