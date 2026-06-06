target_include_directories(stmepic_base INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/stmepic_base> 
)

target_sources(stmepic_base INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/i2c.cpp
)