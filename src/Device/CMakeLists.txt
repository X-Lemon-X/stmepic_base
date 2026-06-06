

target_include_directories(stmepic_base INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/stmepic_base> 
)

target_sources(stmepic_base PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/device.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/simple_task.cpp
)
