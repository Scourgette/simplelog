cmake_minimum_required (VERSION 3.14)
project (simplelog LANGUAGES CXX VERSION 0.1)

# Prerequisites
include(CTest)
include(cmake/simplelog.config.cmake)

# Obj library
include(cmake/simplelog.src.cmake)
add_library(simplelog_obj OBJECT ${SRCS})

# Shared library
set_property(TARGET simplelog_obj PROPERTY POSITION_INDEPENDENT_CODE ON)
add_library(simplelog SHARED $<TARGET_OBJECTS:simplelog_obj>)
add_library(simplelog::simplelog ALIAS simplelog)

# Static library
add_library(simplelog_static STATIC $<TARGET_OBJECTS:simplelog_obj>)
add_library(simplelog_whole INTERFACE)
target_link_libraries(simplelog_whole INTERFACE -Wl,--whole-archive simplelog_static -Wl,--no-whole-archive)
add_library(simplelog::simplelog_static ALIAS simplelog_whole)

# Add dependencies, tests, and installation targets
include(cmake/simplelog.dependencies.cmake)
include(cmake/simplelog.tests.cmake)
include(cmake/simplelog.install.cmake)

foreach(lib simplelog simplelog_static simplelog_obj)
  # C++ 14
  set_target_properties(${lib} PROPERTIES CXX_STANDARD 14)
  # Add definitions
  target_compile_definitions(${lib} INTERFACE ${DEFINITIONS})
  # Don't accept any warning
  if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    target_compile_options(${lib} PRIVATE -Wall -Wextra)
  elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    target_compile_options(${lib} PRIVATE /EHsc /MTd /W2 /c)
  endif()
endforeach()

# Export include directory
foreach(lib simplelog simplelog_static)
  target_include_directories(${lib} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  )
endforeach()
