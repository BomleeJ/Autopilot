cmake_minimum_required(VERSION 3.14)

project(C172Autopilot
    VERSION 0.1
    LANGUAGES CXX
)

# -----------------------
# Global C++ settings
# -----------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# -----------------------
# Paths
# -----------------------
set(XPLANE_SDK ${PROJECT_SOURCE_DIR}/SDK)

# -----------------------
# Includes
# -----------------------
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${XPLANE_SDK}/CHeaders/XPLM
    ${XPLANE_SDK}/CHeaders/Widgets
    /opt/homebrew/opt/nlohmann-json/include
)

# -----------------------
# Source files
# -----------------------
file(GLOB PLUGIN_SOURCES
    src/*.cpp
)

# -----------------------
# X-Plane Plugin Target
# -----------------------
add_library(C172Autopilot MODULE
    ${PLUGIN_SOURCES}
)

target_compile_definitions(C172Autopilot PRIVATE
    APL=1
    XPLM200
    XPLM210
    XPLM300
    XPLM301
)

if(APPLE)
    target_link_libraries(C172Autopilot
        "-F${XPLANE_SDK}/Libraries/Mac"
        "-framework XPLM"
        "-framework XPWidgets"
        "-framework OpenGL"
        "-framework CoreFoundation"
    )
    set_target_properties(C172Autopilot PROPERTIES SUFFIX ".xpl")
endif()

# -----------------------
# GoogleTest
# -----------------------
include(FetchContent)

FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

# -----------------------
# Test Target (AIO)
# -----------------------
add_executable(AIO
    tests/testAircraftIO.cpp
    src/AircraftIO.cpp     # logic you want to test
)

# Same defines as plugin (pragmatic choice)
target_compile_definitions(AIO PRIVATE
    APL=1
    XPLM200
    XPLM210
    XPLM300
    XPLM301
    UNIT_TEST
)

target_link_libraries(AIO
    gtest_main
)

add_test(
    NAME AIO_tests
    COMMAND AIO
)