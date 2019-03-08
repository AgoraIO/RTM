cmake_minimum_required (VERSION 2.8)
project(rtm_sdk_demo)

set(TARGET_NAME rtmServerDemo)
set(SOURCES rtmServerDemo.cpp)
set(HEADERS)

set(TARGET_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "-fPIC -O2 -g -std=c++11 -msse2")

include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${TARGET_NAME} agora_rtm_sdk pthread)



