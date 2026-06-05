cmake_minimum_required(VERSION 3.10)

project(stmepic_base)

# Set the upper project name
set(UPPER_PROJECT_NAME ${CMAKE_PROJECT_NAME})

# Create the main library target  
add_library(${PROJECT_NAME} STATIC)



# Create an alias for the library
add_library(stmepic_base::stmepic_base ALIAS ${PROJECT_NAME})

# Add subdirectories (fixing duplicate entry)
add_subdirectory(src/Timing)
add_subdirectory(src/Inc)
add_subdirectory(src/Device)
add_subdirectory(src/Algorithm)
add_subdirectory(src/Hardware)
add_subdirectory(src/Sensors)
add_subdirectory(src/Memory)
add_subdirectory(src/Controllers)

# Link libraries (empty for now, but properly set up)
target_link_libraries(${PROJECT_NAME} INTERFACE)
