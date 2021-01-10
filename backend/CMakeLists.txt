cmake_minimum_required(VERSION 3.5)

project(arth)

set (CMAKE_CXX_STANDARD 17)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

message(STATUS "compiler: " ${CMAKE_CXX_COMPILER_ID})
include(CheckCXXSourceRuns)

# this is to find packages installed using cmake
# if there are other custom paths, add here
# it is assumed that drogon is installed with all dependencies                                                            
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/usr/local/lib/cmake/Drogon/" "/usr/local/lib/cmake/Trantor")

message(STATUS "${CMAKE_MODULE_PATH}")

set(arth_sources
    src/main.cc
    src/api/v1/register.cc
    src/api/v1/login.cc)

add_executable(${PROJECT_NAME} ${arth_sources})
target_include_directories(arth PRIVATE "/usr/include/jsoncpp")


find_package(OpenSSL REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(ZLIB REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_library( DL_LIBRARY libdl.so  REQUIRD)
find_package(Drogon REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE drogon trantor ${DL_LIBRARY} Threads::Threads uuid jsoncpp cares brotlicommon z brotlienc brotlidec pq sqlite3 ssl crypto jwt-cpp::jwt-cpp)
