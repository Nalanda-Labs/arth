cmake_minimum_required(VERSION 3.5)

project(arth)

message(STATUS "compiler: " ${CMAKE_CXX_COMPILER_ID})
include(CheckCXXSourceRuns)

# this is to find packages installed using cmake
# if there are other custom paths, add here
# it is assumed that drogon is installed with all dependi                                                            
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /usr/local/lib/cmake/)

find_package(Drogon REQUIRED)
find_package(Trantor REQUIRED)

set(arth_sources
    src/main.cc)

add_executable(${PROJECT_NAME} ${arth_sources})

find_package(OpenSSL)
if(OpenSSL_FOUND)
  target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif(OpenSSL_FOUND)

target_link_libraries(${PROJECT_NAME} drogon trantor)