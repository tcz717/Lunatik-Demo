cmake_minimum_required(VERSION 3.5)

project(libluaudp)

find_package(Lua 5.3 REQUIRED)
include_directories(${LUA_INCLUDE_DIR})
set(LIBS ${LIBS} ${LUA_LIBRARIES})

add_library(luaudp SHARED src/luaudp.c)

file(GLOB TESTS test/ test/*.lua)

enable_testing()
set(ENV{LUA_CPATH} ENV{LUA_CPATH} ${CMAKE_BUILD_DIRECTORY})
foreach(t ${TESTS})
    add_test(NAME ${t} COMMAND lua5.3 ${t})
    set_tests_properties(${t} PROPERTIES DEPENDS luaudp)
endforeach()