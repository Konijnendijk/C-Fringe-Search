
cmake_minimum_required(VERSION 3.1)

option(BUILD_TESTS "Build the tests" FALSE)

set(SOURCE_FILES src/FringeSearch.cpp src/FringeGraph.cpp)
set(HEADER_FILES include/FringeSearch.h include/FringeGraph.h)

# Also include header files to let them show up in IDEs
add_library(FringeSearch STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(FringeSearch PUBLIC include)

set_property(TARGET FringeSearch PROPERTY CXX_STANDARD 11)

if (BUILD_TESTS)
    add_subdirectory(test)
endif()