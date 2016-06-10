cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -DBOOST_LOG_DYN_LINK")

find_package( Boost COMPONENTS system thread log filesystem  REQUIRED)
#find_package( Protobuf REQUIRED)
#set(cmake_cxx_ldflags,"-lpthread")

#if(BOOST_FOUNDED)
#    find_library( system )
#    find_library( thread )
#endif()
include_directories( ${Boost_INCLUDE_DIR} )
link_directories( ${Boost_LIBRARY_DIRS} )
set(Boost_USE_STATIC_LIBS    OFF )
set(Boost_USE_MULTITHREADED   ON )
set(Boost_USE_STATIC_RUNTIME OFF )
set(Boost_ALL_DYN_LINK        ON )

add_executable(server server.cc fileinfo.cc) 
target_link_libraries(server ${Boost_LIBRARIES} )
#target_link_libraries(server ${Boost_LIBRARIES} ${PROTOBUF_LIBRARIES} )
add_executable(client client.cc fileinfo.cc )
target_link_libraries(client ${Boost_LIBRARIES} )
#target_link_libraries(client ${Boost_LIBRARIES} ${PROTOBUF_LIBRARIES} )
#file(GLOB SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cc)
#file(GLOB HEAD_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)

