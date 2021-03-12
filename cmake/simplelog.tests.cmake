if (BUILD_TESTING)
  set(TESTS
    tests/config.cpp
    tests/config_parser.cpp
  )

  fetch(googletest "https://github.com/google/googletest.git" "master")
  add_executable(simplelog-tests ${TESTS})
  target_link_libraries(simplelog-tests simplelog::simplelog gtest gtest_main gmock)
  set_target_properties(simplelog-tests PROPERTIES CXX_STANDARD 14)
endif()
