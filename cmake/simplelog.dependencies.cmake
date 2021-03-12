include(FetchContent)
function (fetch libname repo tag)
  if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../${libname}/CMakeLists.txt")
    ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/../${libname} ${CMAKE_CURRENT_BINARY_DIR}/${libname})
  else ()
    FetchContent_Declare(${libname} GIT_REPOSITORY "${repo}" GIT_TAG "${tag}")
    FetchContent_MakeAvailable(${libname})
  endif ()
endfunction ()

# Fetch dependencies
fetch("fmt" "https://github.com/fmtlib/fmt.git" "master")

foreach(lib simplelog_obj simplelog simplelog_static)
  # Link with fmt
  target_link_libraries(${lib} "fmt::fmt-header-only")

  # Needed system dependencies
  if (WIN32)
    target_link_libraries(${lib} ws2_32)
  else ()
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(${lib} Threads::Threads)
  endif ()
endforeach()
