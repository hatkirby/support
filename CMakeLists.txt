cmake_minimum_required (VERSION 3.1)
project (support)

find_package(PkgConfig)
pkg_check_modules(yaml-cpp yaml-cpp REQUIRED)

add_subdirectory(vendor/verbly)
add_subdirectory(vendor/libtwittercpp)

include_directories(
  vendor/verbly/lib
  vendor/libtwittercpp/src
  ${yaml-cpp_INCLUDE_DIRS})

add_executable(support sentence.cpp support.cpp)
set_property(TARGET support PROPERTY CXX_STANDARD 11)
set_property(TARGET support PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(support verbly twitter++ ${yaml-cpp_LIBRARIES})
