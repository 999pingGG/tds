cmake_minimum_required(VERSION 3.17)

project(tests C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(tests
  src/tests.c
  include/tds/private/common.h
  include/tds/private/begin.inc
  include/tds/private/end.inc
  include/tds/vector.h
  libs/munit/munit.c
  libs/munit/munit.h
)

if(MSVC)
  target_compile_options(tests PRIVATE /W4 /WX)
else()
  target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
  target_compile_definitions(tests PRIVATE MUNIT_NO_FORK)
endif()

target_include_directories(tests PRIVATE include libs/munit)
