cmake_minimum_required(VERSION 3.16)

project(popl2020-tutorial LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ standard to conform to")

find_package(LLVM REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions)
include(AddLLVM)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

set(Z3_DIR ${PROJECT_SOURCE_DIR}/z3-4.11.0)
get_filename_component(Z3_DIR ${Z3_DIR} ABSOLUTE)
set(Z3_CXX_INCLUDE_DIRS ${Z3_DIR}/include)

message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME} ${CMAKE_HOST_SYSTEM_PROCESSOR}")
if (APPLE AND CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL arm64)
    set(Z3_LIBRARIES ${Z3_DIR}/lib/libz3_darwin_arm64.a)
elseif (APPLE AND CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL x86_64)
    set(Z3_LIBRARIES ${Z3_DIR}/lib/libz3_darwin_x86_64.a)
elseif (CMAKE_SYSTEM_NAME STREQUAL Linux AND CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL x86_64)
    set(Z3_LIBRARIES ${Z3_DIR}/lib/libz3_linux_x86_64.a)
else ()
    message(FATAL_ERROR "Sorry, your system isn't supported: ${CMAKE_SYSTEM} ${CMAKE_HOST_SYSTEM_PROCESSOR}")
endif ()

message(STATUS "Z3_LIBRARIES: ${Z3_LIBRARIES}")

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS} include)
include_directories(${Z3_CXX_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

include(FetchContent)
FetchContent_Declare(
        GoogleTest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(GoogleTest)

llvm_map_components_to_libnames(llvm_libs support core irreader)

enable_testing()

add_subdirectory(ex1)
# add_subdirectory(ex2)
add_subdirectory(ex3)
