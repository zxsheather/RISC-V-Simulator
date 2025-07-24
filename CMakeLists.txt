cmake_minimum_required(VERSION 3.20)
project(simulator)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Collect all source files
file(GLOB_RECURSE SRC_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "include/*.h" "src/*.hpp")

# Main simulator executable
add_executable(simulator ${SRC_FILES})
target_compile_definitions(simulator PRIVATE _DEBUG)

# Optional: Create a static library for the framework (excluding main.cpp)
file(GLOB_RECURSE FRAMEWORK_SRC "src/*.cpp")
list(REMOVE_ITEM FRAMEWORK_SRC "${CMAKE_SOURCE_DIR}/src/main.cpp")

add_library(risc_v_framework STATIC ${FRAMEWORK_SRC})
target_include_directories(risc_v_framework PUBLIC include)
target_compile_definitions(risc_v_framework PRIVATE _DEBUG)

# Link the main simulator with the framework library
target_link_libraries(simulator risc_v_framework)

# Set output directories
set_target_properties(simulator risc_v_framework
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Enable testing (optional)
enable_testing()

# Add a test for the simulator
add_test(NAME simulator_test COMMAND simulator)

# Custom target to copy testcases (if they exist)
if(EXISTS "${CMAKE_SOURCE_DIR}/testcases")
    add_custom_target(copy_testcases ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/testcases"
        "${CMAKE_BINARY_DIR}/testcases"
        COMMENT "Copying testcases to build directory"
    )
    add_dependencies(simulator copy_testcases)
endif()

# Print build information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
endif()

# Custom targets for convenience
add_custom_target(run_simulator
    COMMAND ${CMAKE_BINARY_DIR}/bin/simulator
    DEPENDS simulator
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running the RISC-V simulator"
)

add_custom_target(clean_build
    COMMAND ${CMAKE_COMMAND} --build . --target clean
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning build directory"
)

# Install targets (optional)
install(TARGETS simulator risc_v_framework
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

install(FILES README.md docs/help.md docs/frame.md
    DESTINATION share/doc/risc-v-simulator
)