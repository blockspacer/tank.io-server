cmake_minimum_required(VERSION 3.1.0)

add_definitions(-std=gnu++0x)


project(tank-io.server2)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC OF)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC OF)

# Find the QtWidgets library
#find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Sql REQUIRED)


# Populate a CMake variable with the sources
add_definitions(-DSERVER_SIDE)

#add_definitions(-lboost_thread -lboost_system   -lrt)

include_directories(
        Classes
)

file(GLOB helloworld_SRCS1
     "${CMAKE_CURRENT_LIST_DIR}/Engine/*.cpp"
     "${CMAKE_CURRENT_LIST_DIR}/DB/*.cpp"
     "${CMAKE_CURRENT_LIST_DIR}/debuge_tools/*.cpp"
     "${CMAKE_CURRENT_LIST_DIR}/base/*.cpp"
     "${CMAKE_CURRENT_LIST_DIR}/Classes/GameCore/*.cpp"
     "${CMAKE_CURRENT_LIST_DIR}/base_message/*.cpp"
     "${CMAKE_CURRENT_LIST_DIR}/cocos2d_server/*.cpp"
     "${CMAKE_CURRENT_LIST_DIR}/Classes/game_message/*.cpp"
     "${CMAKE_CURRENT_LIST_DIR}/Classes/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/Classes/MathShapeUtils/*.cpp"

)
file(GLOB helloworld_HEAD1
     "${CMAKE_CURRENT_LIST_DIR}/Engine/*.h"
     "${CMAKE_CURRENT_LIST_DIR}/DB/*.h"
     "${CMAKE_CURRENT_LIST_DIR}/debuge_tools/*.h"
     "${CMAKE_CURRENT_LIST_DIR}/base/*.h"
     "${CMAKE_CURRENT_LIST_DIR}/Classes/GameCore/*.h"
     "${CMAKE_CURRENT_LIST_DIR}/base_message/*.h"
     "${CMAKE_CURRENT_LIST_DIR}/cocos2d_server/*.h"
     "${CMAKE_CURRENT_LIST_DIR}/Classes/game_message/*.h"
     "${CMAKE_CURRENT_LIST_DIR}/Classes/*.h"

)



set(helloworld_SRCS
    ${helloworld_SRCS1}
    ${helloworld_HEAD1}
    server_delegator.cpp
)

# Tell CMake to create the helloworld executable
add_executable(tank-io.server ${helloworld_SRCS})
# Use the Widgets module from Qt 5
#target_link_libraries(helloworld Qt5::Widgets)
target_link_libraries(tank-io.server rt boost_thread boost_system pthread)
target_link_libraries(tank-io.server Qt5::Core  Qt5::Sql)

#LIBS +=  -lboost_thread -lboost_system   -lrt
