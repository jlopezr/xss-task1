cmake_minimum_required (VERSION 2.8.11)
project (Task1)

include(${PROJECT_SOURCE_DIR}/EnableCXX11.cmake)

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost REQUIRED COMPONENTS system thread) #COMPONENTS filesystem
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable (server server.cpp)
    target_link_libraries(server ${Boost_LIBRARIES})
    #add_executable (server-mt server-mt.cpp)
    #target_link_libraries(server ${Boost_LIBRARIES})
    add_executable (client client.cpp)
    target_link_libraries(client ${Boost_LIBRARIES})
endif()

