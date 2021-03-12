# Install static and shared targets
install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(TARGETS simplelog simplelog_static EXPORT simplelog
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Install pkgconfig
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/simplelog.pc.in ${CMAKE_CURRENT_BINARY_DIR}/simplelog.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/simplelog.pc DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

