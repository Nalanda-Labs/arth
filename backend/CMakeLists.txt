cmake_minimum_required(VERSION 3.5)

project(arth)

set (CMAKE_CXX_STANDARD 20)

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
    src/api/v1/login.cc
    src/api/v1/forgot_password.cc
    src/api/v1/plugins/email.cc
    src/api/v1/topic.cc
    src/api/v1/profile.cc
    src/api/v1/tags.cc
    src/api/v1/util/arth.cc
    src/api/v1/stats.cc
	src/api/v1/posts.cc
    )

add_executable(${PROJECT_NAME} ${arth_sources})
target_include_directories(arth PRIVATE "/usr/include/")
target_include_directories(arth PRIVATE "/usr/include/jsoncpp/")


find_package(OpenSSL REQUIRED)
#find_package(Jsoncpp REQUIRED)
find_package(ZLIB REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_library( DL_LIBRARY libpq.so  REQUIRED)
find_library( DL_LIBRARY libdl.so  REQUIRED)
find_library( DL_LIBRARY libargon2.a  REQUIERED)
find_library( DL_LIBRARY libbsd.so  REQUIERED)
find_package(Drogon REQUIRED)
find_library(argon2 REQUEIRD)

target_link_libraries(${PROJECT_NAME} PRIVATE drogon trantor ${DL_LIBRARY} 
	Threads::Threads uuid jsoncpp cares brotlicommon z brotlienc brotlidec
	pq ssl crypto argon2 bsd dl sqlite3)
