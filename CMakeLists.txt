# CMakeList.txt : CMake project for arbitray_precision_number, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project (arbitray_precision_number VERSION 0.1 LANGUAGES CXX)

add_executable (ap_test "src/test.cpp" "src/ap_number.h" "src/consts.cpp")
add_executable (ap_example "src/example.cpp" "src/ap_number.h" "src/consts.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ap_test PROPERTY CXX_STANDARD 17)
  set_property(TARGET ap_example PROPERTY CXX_STANDARD 17)
endif()

